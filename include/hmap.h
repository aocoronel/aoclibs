#ifndef AOC_HASHMAP_H_
#define AOC_HASHMAP_H_

#include "base.h"

#include "cstr.h"
#include "da.h"

#define HASHMAP(type)                   \
	struct {                            \
		DYNAMIC_ARRAY(Hash_Entry_Tmpl); \
	} entries;                          \
	DYNAMIC_ARRAY(type);

struct Hash_Entry_Tmpl {
	Slice key;
	uint64_t hash;
	uint32_t meta;
	size_t value_idx;
};

struct Hash_Map_Tmpl {
	HASHMAP(void *);
};

#define hmap_init(map, capacity) \
	(typeof((map)))_hmap_init((Hash_Map_Tmpl *)map, (capacity), sizeof(*(map)->entries.data))
AOCDEF bool _hmap_init(Hash_Map_Tmpl *map, const size_t capacity, const size_t sizeof_entry);

#define hmap_resize(map, new_capacity) _hmap_resize((Hash_Map_Tmpl *)map, (new_capacity))
AOCDEF bool _hmap_resize(Hash_Map_Tmpl *map, size_t new_capacity);

#define hmap_free(map) _hmap_free((Hash_Map_Tmpl *)map)
AOCDEF void _hmap_free(Hash_Map_Tmpl *map);

#define hmap_clear _hmap_clear((Hash_Map_Tmpl *)map)
AOCDEF void _hmap_clear(Hash_Map_Tmpl *map);

#define hmap_reset _hmap_reset((Hash_Map_Tmpl *)map)
AOCDEF void _hmap_reset(Hash_Map_Tmpl *map);

#define hmap_insert(map, slice, val)                                                  \
	({                                                                                \
		bool ok = false;                                                              \
		typeof((map)->entries.data) e;                                                \
		e = (typeof((map)->entries.data))_hmap_insert((Hash_Map_Tmpl *)map, (slice)); \
		if (e) {                                                                      \
			da_insert((map), (val));                                                  \
			e->value_idx = ((map)->len - 1);                                          \
			ok = true;                                                                \
		}                                                                             \
		ok;                                                                           \
	})
AOCDEF void *_hmap_insert(Hash_Map_Tmpl *map, const Slice key);
AOCDEF void *_hmap_insert_from_hash(Hash_Map_Tmpl *map, const Slice key, const uint64_t hash);

#define hmap_get(map, slice)                                   \
	({                                                         \
		size_t idx = _hmap_get((Hash_Map_Tmpl *)map, (slice)); \
		$assert(idx != SIZE_MAX);                              \
		(map)->data[idx];                                      \
	})
#define hmap_get_value(map, slice)                             \
	({                                                         \
		typeof((map)->data) ret = NULL;                        \
		size_t idx = _hmap_get((Hash_Map_Tmpl *)map, (slice)); \
		if (idx != SIZE_MAX) {                                 \
			ret = (map)->data + idx;                           \
		}                                                      \
		ret;                                                   \
	})
AOCDEF size_t _hmap_get(const Hash_Map_Tmpl *map, const Slice key);
AOCDEF Hash_Entry_Tmpl *
_hmap_get_from_hash(const Hash_Map_Tmpl *map, const Slice key, const uint64_t hash);
AOCDEF size_t _hmap_get_value_from_hash(const Hash_Map_Tmpl *map,
										const Slice key,
										const uint64_t hash);

#define hmap_remove(map, slice) _hmap_remove((Hash_Map_Tmpl *)map, (slice))

AOCDEF bool _hmap_remove(Hash_Map_Tmpl *map, const Slice key);
AOCDEF bool _hmap_remove_from_hash(Hash_Map_Tmpl *map, const uint64_t hash, const Slice key);

#define hmap_iterate(map, count, index)                        \
	({                                                         \
		Hash_Entry_Tmpl *result = NULL;                        \
		if (count <= (map)->size) {                            \
			size_t cap = (map)->cap;                           \
			while (index < cap) {                              \
				Hash_Entry_Tmpl *e = &(map)->entries[index++]; \
				if (hmap_entry_is_used(e)) {                   \
					count++;                                   \
					result = e;                                \
					break;                                     \
				}                                              \
			}                                                  \
			if (index >= cap) $assert(0, "iterator overflow"); \
		}                                                      \
		result;                                                \
	})

#ifdef AOC_IMPLEMENTATION
#include "hmap.c"
#endif

#endif // AOC_HASHMAP_H_
