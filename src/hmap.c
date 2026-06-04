#pragma once

#include "base.h"
#include "calculus.h"
#include "hmap.h"
#include "rapidhash.h"

#define _HMAP_META_USED_MASK 0x80000000u
#define _HMAP_META_DISTANCE_MASK 0x7fffffffu
#define _HMAP_ENTRY_IS_USED(entry) ((entry->meta & _HMAP_META_USED_MASK) != 0)
#define _HMAP_ENTRY_DISTANCE(entry) (entry->meta & _HMAP_META_DISTANCE_MASK)
#define _HMAP_SET_DISTANCE(entry, distance) \
    (entry)->meta = _HMAP_META_USED_MASK | ((distance) & _HMAP_META_DISTANCE_MASK);
#define _HMAP_MASK(map) (map)->cap - 1
#define _HMAP_HASH(slice) rapidhash(key.data, key.len)
#define _HMAP_INDEX(map, hash) (size_t)(hash) & _HMAP_MASK((map))

bool _hmap_resize(_HashMap *map, size_t new_capacity) {
    ASSERT_NONNULL(map);
    ASSERT_NONNULL(map->entries);

    size_t cap = coerce_to_pow2(new_capacity);
    ASSERT(cap - 1 <= _HMAP_META_DISTANCE_MASK);

    _HashEntry *new_entries = (_HashEntry *)calloc(cap, sizeof(_HashEntry));
    if (!new_entries) return false;

    _HashEntry *old_entries = map->entries;
    size_t old_capacity = map->cap;

    _HashMap new_map;
    new_map.entries = new_entries;
    new_map.cap = cap;
    new_map.len = 0;

    for (size_t i = 0; i < old_capacity; i++) {
        _HashEntry *e = &old_entries[i];
        if (_HMAP_ENTRY_IS_USED(e)) {
            _HashEntry *_e = _hmap_insert_from_hash(&new_map, e->key, e->hash);
            _e->value = e->value;
        }
    }

    free(old_entries);
    *map = new_map;
    return true;
}

void _hmap_free(_HashMap *map) {
    ASSERT_NONNULL(map);
    ASSERT_NONNULL(map->entries);
    if (!map || !map->entries) return;
    free(map->entries);
    map->entries = NULL;
    map->cap = 0;
    map->len = 0;
}

void _hmap_clear(_HashMap *map) {
    ASSERT_NONNULL(map);
    ASSERT_NONNULL(map->entries);
    if (map->len == 0) return;
    memset(map->entries, 0, map->cap * sizeof(_HashEntry));
    map->len = 0;
}

bool _hmap_remove(_HashMap *map, const Slice key) {
    uint64_t hash = _HMAP_HASH(key);
    return _hmap_remove_from_hash(map, hash, key);
}

bool _hmap_remove_from_hash(_HashMap *map, const uint64_t hash, const Slice key) {
    ASSERT_NONNULL(map);
    ASSERT_NONNULL(map->entries);

    if (map->len == 0) return false;
    size_t idx = _HMAP_INDEX(map, hash);
    size_t capmask = _HMAP_MASK(map);

    for (uint32_t d = 0;; d++) {
        size_t pos = (idx + d) & capmask;
        _HashEntry *e = &map->entries[pos];
        if (!_HMAP_ENTRY_IS_USED(e) || _HMAP_ENTRY_DISTANCE(e) < d) {
            return false;
        }
        if (e->hash != hash || !slice_eq(&e->key, &key)) continue;

        size_t cur = pos;
        for (;;) {
            size_t next = (cur + 1) & capmask;
            _HashEntry *next_e = &map->entries[next];
            if (!_HMAP_ENTRY_IS_USED(next_e)) {
                map->entries[cur].meta = 0;
                map->len--;

                if (map->cap > 16 && map->len * 8 < map->cap) {
                    if (!hmap_resize(map, map->cap / 2)) return false;
                }
                return true;
            }
            uint32_t nd = _HMAP_ENTRY_DISTANCE(next_e);
            if (nd == 0) {
                map->entries[cur].meta = 0;
                map->len--;

                if (map->cap > 16 && map->len * 8 < map->cap) {
                    if (!hmap_resize(map, map->cap / 2)) return false;
                }
                return true;
            }
            map->entries[cur] = *next_e;
            _HMAP_SET_DISTANCE(&map->entries[cur], nd - 1);
            cur = next;
        }
    }
    return true;
}

_HashEntry *_hmap_get(const _HashMap *map, const Slice key) {
    uint64_t hash = _HMAP_HASH(key);
    _HashEntry *e = _hmap_get_from_hash(map, key, hash);
    ASSERT(e && "key not found");
    return e;
}

_HashEntry *_hmap_get_entry(const _HashMap *map, const Slice key) {
    uint64_t hash = _HMAP_HASH(key);
    return _hmap_get_from_hash(map, key, hash);
}

_HashEntry *_hmap_get_from_hash(const _HashMap *map, const Slice key, const uint64_t hash) {
    ASSERT_NONNULL(map);
    ASSERT_NONNULL(map->entries);
    if (map->len == 0) return NULL;

    size_t capmask = _HMAP_MASK(map);
    size_t idx = _HMAP_INDEX(map, hash);

    for (uint32_t d = 0;; d++) {
        _HashEntry *e = &map->entries[(idx + d) & capmask];
        if (!_HMAP_ENTRY_IS_USED(e)) return NULL;

        if (_HMAP_ENTRY_DISTANCE(e) < d) return NULL;

        if (e->hash == hash && slice_eq(&e->key, &key)) return e;
    }
}

bool _hmap_init(_HashMap *map, const size_t capacity, const size_t sizeof_entry) {
    ASSERT_NONNULL(map);
    size_t cap = coerce_to_pow2(capacity);
    ASSERT(cap - 1 <= _HMAP_META_DISTANCE_MASK);

    map->entries = (_HashEntry *)calloc(cap, sizeof_entry);
    if (!map->entries) return false;

    map->cap = cap;
    map->len = 0;
    return true;
}

_HashEntry *_hmap_insert(_HashMap *map, const Slice key, const size_t sizeof_entry) {
    if (!map || !map->entries) {
        if (!hmap_init(map, AOCLIBS_HASHMAP_INITIAL_CAPACITY)) return NULL;
    }

    uint64_t hash = _HMAP_HASH(key);
    _HashEntry *existing = (_HashEntry *)_hmap_get_from_hash(map, key, hash);
    if (existing) {
        existing->key = key;
        return existing;
    }

    if (map->len >= map->cap) return false;

    if (map->len * 4 >= map->cap * 3) {
        if (!hmap_resize(map, map->cap * 2) && map->len + 1 >= map->cap) return NULL;
    }

    return _hmap_insert_from_hash(map, key, hash);
}

_HashEntry *_hmap_insert_from_hash(_HashMap *map, const Slice key, const uint64_t hash) {
    ASSERT_NONNULL(map);
    ASSERT_NONNULL(map->entries);
    size_t idx = _HMAP_INDEX(map, hash);
    uint32_t distance = 0;
    size_t capmask = _HMAP_MASK(map);

    _HashEntry incoming;
    incoming.key = key;
    incoming.hash = hash;
    _HMAP_SET_DISTANCE(&incoming, distance);

    for (size_t offset = 0; offset < map->cap; offset++, distance++) {
        size_t pos = (idx + offset) & capmask;
        _HashEntry *e = &map->entries[pos];

        if (!_HMAP_ENTRY_IS_USED(e)) {
            _HMAP_SET_DISTANCE(&incoming, distance);
            *e = incoming;
            map->len++;
            return e;
        }

        if (e->hash == hash && slice_eq(&e->key, &key)) {
            e->value = incoming.value;
            e->key = incoming.key;
            return e;
        }

        uint32_t existing_distance = _HMAP_ENTRY_DISTANCE(e);

        if (existing_distance < distance) {
            _HMAP_SET_DISTANCE(&incoming, distance);
            swap(*e, incoming);
            distance = existing_distance;
        }
    }
    UNREACHABLE;
}

#ifdef TUNIT
typedef struct HashEntry {
    Slice key;
    uint64_t hash;
    uint32_t meta;
    float value;
} HashEntry;

typedef struct HashMap {
    size_t cap;
    size_t len;
    _HashEntry *entries;
} HashMap;

TEST(hashmap_test) {
    HashMap map = { 0 };
    // Optional, AOCLIBS_HASHMAP_INITIAL_CAPACITY will be used if this step isn't done
    if (!hmap_init(&map, 10)) {
        TASSERT(0, "out of memory");
    }
    size_t count = 0;
    size_t index = 0;

    Slice hello = { .data = "hello", .len = strlen("hello") };
    Slice world = { .data = "world", .len = strlen("world") };
    Slice world2 = { .data = "world2", .len = strlen("world2") };
    Slice world3 = { .data = "world3", .len = strlen("world3") };

    hmap_insert(&map, hello, 123);
    hmap_insert(&map, world, 456);
    hmap_insert(&map, world2, 456);
    hmap_insert(&map, world3, 456);

    TASSERT(hmap_get(&map, hello) == 123, "values don't match");
    TASSERT(hmap_get(&map, world) == 456, "values don't match");
    TASSERT(hmap_get(&map, world2) == 456, "values don't match");
    TASSERT(hmap_get(&map, world3) == 456, "values don't match");

    hmap_remove(&map, hello);
    TASSERT(hmap_get_entry(&map, hello) == NULL, "value was supposed to be removed");

    hmap_free(&map);
}
#endif
