#ifndef AOCLIBS_HASHMAP_H_
#define AOCLIBS_HASHMAP_H_

#include "base.h"
#include "cstr.h"

#define AOCLIBS_HASHMAP_INITIAL_CAPACITY 1024

typedef struct _HashEntry {
	Slice key;
	uint64_t hash;
	uint32_t meta;
	int value;
} _HashEntry;

typedef struct _HashMap {
	size_t cap;
	size_t len;
	_HashEntry *entries;
} _HashMap;

#define hmap_init(map, capacity) \
	(__typeof__((map)))_hmap_init((_HashMap *)map, (capacity), sizeof(*(map)->entries))
AOCDEF bool _hmap_init(_HashMap *map, const size_t capacity, const size_t sizeof_entry);

#define hmap_resize(map, new_capacity) _hmap_resize((_HashMap *)map, (new_capacity))
AOCDEF bool _hmap_resize(_HashMap *map, size_t new_capacity);

#define hmap_free(map) _hmap_free((_HashMap *)map)
AOCDEF void _hmap_free(_HashMap *map);

#define hmap_clear _hmap_clear((_HashMap *)map)
AOCDEF void _hmap_clear(_HashMap *map);

#define hmap_insert(map, slice, val)                                             \
	({                                                                           \
		bool ok = false;                                                         \
		__typeof__((map)->entries) e = (__typeof__((map)->entries))_hmap_insert( \
				(_HashMap *)map, (slice), sizeof(*(map)->entries));              \
		if (e) {                                                                 \
			e->value = (val);                                                    \
			ok = true;                                                           \
		}                                                                        \
		ok;                                                                      \
	})
AOCDEF _HashEntry *_hmap_insert(_HashMap *map, const Slice key, const size_t sizeof_entry);
AOCDEF _HashEntry *_hmap_insert_from_hash(_HashMap *map, const Slice key, const uint64_t hash);

#define hmap_get(map, slice) \
	((__typeof__((map)->entries))_hmap_get((_HashMap *)map, (slice)))->value
#define hmap_get_entry(map, slice) \
	((__typeof__((map)->entries))_hmap_get_entry((_HashMap *)map, (slice)))
AOCDEF _HashEntry *_hmap_get(const _HashMap *map, const Slice key);
AOCDEF _HashEntry *_hmap_get_entry(const _HashMap *map, const Slice key);
AOCDEF _HashEntry *_hmap_get_from_hash(const _HashMap *map, const Slice key, const uint64_t hash);

#define hmap_remove(map, slice) _hmap_remove((_HashMap *)map, (slice))

AOCDEF bool _hmap_remove(_HashMap *map, const Slice key);
AOCDEF bool _hmap_remove_from_hash(_HashMap *map, const uint64_t hash, const Slice key);

#define hmap_iterate(map, count, index)                       \
	({                                                        \
		_HashEntry *result = NULL;                            \
		if (count <= (map)->size) {                           \
			size_t cap = (map)->cap;                          \
			while (index < cap) {                             \
				_HashEntry *e = &(map)->entries[index++];     \
				if (hmap_entry_is_used(e)) {                  \
					count++;                                  \
					result = e;                               \
					break;                                    \
				}                                             \
			}                                                 \
			if (index >= cap) ASSERT(0, "iterator overflow"); \
		}                                                     \
		result;                                               \
	})

#ifdef AOCLIBS_IMPLEMENTATION
#include "hmap.c"
#endif

#endif // AOCLIBS_HASHMAP_H_
