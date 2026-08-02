#pragma once

#include "base.h"
#include "calculus.h"
#include "hmap.h"
#include "da.h"
#include "rapidhash.h"
#include <string.h>

enum {
	_HMAP_META_USED_MASK = 0x80000000u,
	_HMAP_META_DISTANCE_MASK = 0x7fffffffu,
};

#define _HMAP_ENTRY_IS_USED(entry) ((entry->meta & _HMAP_META_USED_MASK) != 0)
#define _HMAP_ENTRY_DISTANCE(entry) (entry->meta & _HMAP_META_DISTANCE_MASK)
#define _HMAP_SET_DISTANCE(entry, distance) \
	(entry)->meta = _HMAP_META_USED_MASK | ((distance) & _HMAP_META_DISTANCE_MASK);
#define _HMAP_MASK(map) (map)->entries.cap - 1
#define _HMAP_HASH(slice) rapidhash(key.data, key.len)
#define _HMAP_INDEX(map, hash) (size_t)(hash) & _HMAP_MASK((map))

bool _hmap_resize(Hash_Map_Tmpl *map, size_t new_capacity) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);

	size_t cap = coerce_to_pow2(new_capacity);
	$assert(cap - 1 <= _HMAP_META_DISTANCE_MASK);

	Hash_Entry_Tmpl *new_entries = (Hash_Entry_Tmpl *)calloc(cap, sizeof(Hash_Entry_Tmpl));
	$catch(!new_entries) return false;

	Hash_Entry_Tmpl *old_entries = map->entries.data;
	size_t old_capacity = map->entries.cap;

	Hash_Map_Tmpl new_map;
	new_map.entries.data = new_entries;
	new_map.entries.cap = cap;
	new_map.entries.len = 0;

	for (size_t i = 0; i < old_capacity; i++) {
		Hash_Entry_Tmpl *e = &old_entries[i];
		if (_HMAP_ENTRY_IS_USED(e)) {
			Hash_Entry_Tmpl *_e = (Hash_Entry_Tmpl *)_hmap_insert_from_hash(&new_map, e->key, e->hash);
			_e->value_idx = e->value_idx;
		}
	}

	free(old_entries);
	*map = new_map;
	return true;
}

void _hmap_free(Hash_Map_Tmpl *map) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);
	free(map->entries.data);
	map->entries.data = NULL;
	map->entries.cap = 0;
	map->entries.len = 0;
}

void _hmap_clear(Hash_Map_Tmpl *map) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);
	if (map->entries.len == 0) return;
	memset(map->entries.data, 0, map->entries.cap * sizeof(Hash_Entry_Tmpl));
	map->entries.len = 0;
}

void _hmap_reset(Hash_Map_Tmpl *map) {
	$assert_nonnull(map);
	map->entries.len = 0;
}

bool _hmap_remove(Hash_Map_Tmpl *map, const Slice key) {
	uint64_t hash = _HMAP_HASH(key);
	return _hmap_remove_from_hash(map, hash, key);
}

bool _hmap_remove_from_hash(Hash_Map_Tmpl *map, const uint64_t hash, const Slice key) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);

	$catch(map->entries.len == 0) return false;
	size_t idx = _HMAP_INDEX(map, hash);
	size_t capmask = _HMAP_MASK(map);

	for (uint32_t d = 0;; d++) {
		size_t pos = (idx + d) & capmask;
		Hash_Entry_Tmpl *e = &map->entries.data[pos];
		$catch(!_HMAP_ENTRY_IS_USED(e) || _HMAP_ENTRY_DISTANCE(e) < d) {
			return false;
		}
		if (e->hash != hash || !slice_eq(e->key, key)) continue;

		size_t cur = pos;
		for (;;) {
			size_t next = (cur + 1) & capmask;
			Hash_Entry_Tmpl *next_e = &map->entries.data[next];
			if (!_HMAP_ENTRY_IS_USED(next_e)) {
				map->entries.data[cur].meta = 0;
				map->entries.len--;

				if (map->entries.cap > 16 && map->entries.len * 8 < map->entries.cap) {
					$catch(!hmap_resize(map, map->entries.cap / 2)) return false;
				}
				return true;
			}
			uint32_t nd = _HMAP_ENTRY_DISTANCE(next_e);
			if (nd == 0) {
				map->entries.data[cur].meta = 0;
				map->entries.len--;

				if (map->entries.cap > 16 && map->entries.len * 8 < map->entries.cap) {
					$catch(!hmap_resize(map, map->entries.cap / 2)) return false;
				}
				return true;
			}
			map->entries.data[cur] = *next_e;
			_HMAP_SET_DISTANCE(&map->entries.data[cur], nd - 1);
			cur = next;
		}
	}
	return true;
}

size_t _hmap_get(const Hash_Map_Tmpl *map, const Slice key) {
	uint64_t hash = _HMAP_HASH(key);
	return _hmap_get_value_from_hash(map, key, hash);
}

size_t _hmap_get_value_from_hash(const Hash_Map_Tmpl *map, const Slice key, const uint64_t hash) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);
	if (map->entries.len == 0) return SIZE_MAX;

	size_t capmask = _HMAP_MASK(map);
	size_t idx = _HMAP_INDEX(map, hash);

	for (uint32_t d = 0;; d++) {
		size_t id = (idx + d) & capmask;
		Hash_Entry_Tmpl *e = &map->entries.data[id];
		$catch(!_HMAP_ENTRY_IS_USED(e)) return SIZE_MAX;

		$catch(_HMAP_ENTRY_DISTANCE(e) < d) return SIZE_MAX;

		if (e->hash == hash && slice_eq(e->key, key)) {
			return e->value_idx;
		}
	}
}

Hash_Entry_Tmpl *
_hmap_get_from_hash(const Hash_Map_Tmpl *map, const Slice key, const uint64_t hash) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);
	if (map->entries.len == 0) return NULL;

	size_t capmask = _HMAP_MASK(map);
	size_t idx = _HMAP_INDEX(map, hash);

	for (uint32_t d = 0;; d++) {
		size_t id = (idx + d) & capmask;
		Hash_Entry_Tmpl *e = &map->entries.data[id];
		$catch(!_HMAP_ENTRY_IS_USED(e)) return NULL;

		$catch(_HMAP_ENTRY_DISTANCE(e) < d) return NULL;

		if (e->hash == hash && slice_eq(e->key, key)) {
			return e;
		}
	}
}

bool _hmap_init(Hash_Map_Tmpl *map, const size_t capacity, const size_t sizeof_entry) {
	$assert_nonnull(map);
	size_t cap = coerce_to_pow2(capacity);
	$assert(cap - 1 <= _HMAP_META_DISTANCE_MASK);

	map->entries.data = (Hash_Entry_Tmpl *)calloc(cap, sizeof_entry);
	$catch(!map->entries.data) return false;

	map->entries.cap = cap;
	map->entries.len = 0;
	return true;
}

void hmap_dump(FILE *fd, void *map, const char *map_name, const char *type, Hash_Map_Dump_Fn fn) {
	$assert_nonnull(fn);
	Hash_Map_Tmpl *m = (Hash_Map_Tmpl *)map;
	fprintf(fd, "(%s) {\n", map_name);
	fprintf(fd, "    .entries = {\n");
	fprintf(fd, "        .len = %zu,\n", m->entries.len);
	fprintf(fd, "        .cap = %zu,\n", m->entries.cap);
	fprintf(fd, "        .data = (const Hash_Entry_Tmpl[]) {\n");

	$range(0, m->entries.cap) {
		$assert(it < 259);
		Hash_Entry_Tmpl e = m->entries.data[it];
		if (e.hash == 0) {
			fprintf(fd, "            ");
			for (; it < m->entries.cap; it++) {
				Hash_Entry_Tmpl e = m->entries.data[it];
				if (e.hash != 0) break;
				fprintf(fd, "{ 0 },");
			}
			fprintf(fd, "\n");
			if (it < m->entries.cap) {
				Hash_Entry_Tmpl e = m->entries.data[it];
				fprintf(
				    fd,
				    "            {\n"
				    "                .hash = %zuULL,\n"
				    "                .key = { .data = \"%.*s\", .len = %zu },\n"
				    "                .meta = %u,\n"
				    "                .value_idx = %zu,\n"
				    "            },\n",
				    e.hash, $view_slice(e.key), e.key.len, e.meta, e.value_idx);
			}
			continue;
		}
		fprintf(
		    fd,
		    "            {\n"
		    "                .hash = %zuULL,\n"
		    "                .key = { .data = \"%.*s\", .len = %zu },\n"
		    "                .meta = %u,\n"
		    "                .value_idx = %zu,\n"
		    "            },\n",
		    e.hash, $view_slice(e.key), e.key.len, e.meta, e.value_idx);
	}
	fprintf(fd, "        },\n"); // data
	fprintf(fd, "    },\n"); // entries
	fprintf(
	    fd,
	    "    .len = %zu,\n"
	    "    .cap = %zu,\n",
	    m->len, m->cap);
	fprintf(fd, "    .data = (const %s[]) {\n", type);
	$range(0, m->len) {
		fprintf(fd, "        %s,\n", fn(m->data, it));
	}
	fprintf(fd, "    },\n");
	fprintf(fd, "}\n"); // map_name
}

void *_hmap_insert(Hash_Map_Tmpl *map, const Slice key) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);

	uint64_t hash = _HMAP_HASH(key);
	Hash_Entry_Tmpl *existing = _hmap_get_from_hash(map, key, hash);
	if (existing) return existing;

	$catch(map->entries.len >= map->entries.cap) return NULL;

	if (map->entries.len * 4 >= map->entries.cap * 3) {
		$catch(
		    !hmap_resize(map, map->entries.cap * 2) &&
		    map->entries.len + 1 >= map->entries.cap) return NULL;
	}

	return _hmap_insert_from_hash(map, key, hash);
}

void *_hmap_insert_from_hash(Hash_Map_Tmpl *map, const Slice key, const uint64_t hash) {
	$assert_nonnull(map);
	$assert_nonnull(map->entries.data);
	size_t idx = _HMAP_INDEX(map, hash);
	uint32_t distance = 0;
	size_t capmask = _HMAP_MASK(map);

	Hash_Entry_Tmpl incoming;
	incoming.key = key;
	incoming.hash = hash;
	_HMAP_SET_DISTANCE(&incoming, distance);

	for (size_t offset = 0; offset < map->entries.cap; offset++, distance++) {
		size_t id = (idx + offset) & capmask;
		Hash_Entry_Tmpl *e = &map->entries.data[id];

		if (!_HMAP_ENTRY_IS_USED(e)) {
			_HMAP_SET_DISTANCE(&incoming, distance);
			*e = incoming;
			map->entries.len++;
			return e;
		}

		if (e->hash == hash && slice_eq(e->key, key)) {
			e->key = incoming.key;
			return e;
		}

		uint32_t existing_distance = _HMAP_ENTRY_DISTANCE(e);

		if (existing_distance < distance) {
			_HMAP_SET_DISTANCE(&incoming, distance);
			$swap(*e, incoming);
			distance = existing_distance;
		}
	}
	$unreachable("Table overflew");
}
