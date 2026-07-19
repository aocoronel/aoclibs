#ifndef AOC_MAP_H_
#include "base.h"
#define AOC_MAP_H_

#include "cstr.h"
#include "rc.h"
#include "arena.h"
#include "io.h"

// Map
//
// This is a hash table alternative that doesn't rely on hashing and doesn't require initializing.
//
// It takes a string like "main" and maps each character as a distinct key-value paired table, even
// if not value is ever set.
//
// If we have a map that has been inserted these keys: "dracula", "dragon", "dungeon", "dread",
// "dreadful", the Map would look like this:
//
// d -> d
//    r -> dr
//       a -> dra
//          c -> drac
//             u -> dracu
//                l -> dracul
//                   a -> dracula
//          g -> drag
//             o -> drago
//                n -> dragon
//       e -> dre
//          a -> drea
//             d -> dread
//          e -> dree
//             d -> dreed
//                f -> dreedf
//                   u -> dreedfu
//                      l -> dreedful
//   u -> du
//      n -> dun
//         g -> dung
//            e -> dunge
//               o -> dungeo
//                  n -> dungeon
//
// Lookup is done using binary search

// This is the assumed layout of the map
struct Map_Tmpl {
	size_t len;
	size_t cap;
	Map_Tmpl *data;

	bool occupied;
	char key;
	int value;
};

#define map_dump(map, buff) _map_dump((struct Map_Tmpl *)(map), (buff), 0, 0)

#define map_delete(map, slice)                   \
	({                                           \
		bool result = true;                      \
		typeof(map) c = map_get((map), (slice)); \
		if (!c) {                                \
			result = false;                      \
		} else {                                 \
			(c)->occupied = false;               \
		}                                        \
		result;                                  \
	})

#define map_set(map, slice, val)                 \
	({                                           \
		bool result = true;                      \
		typeof(map) c = map_get((map), (slice)); \
		if (!c) {                                \
			result = false;                      \
		} else {                                 \
			c->value = (val);                    \
		}                                        \
		result;                                  \
	})

#define map_get(map, slice) (typeof(map))_map_get((struct Map_Tmpl *)(map), (slice))

#define map_prepare(arena, map, slice) \
	(typeof(map))_map_prepare((arena), (struct Map_Tmpl *)(map), (slice), sizeof((map)->value))

#define map_insert(arena, map, slice, val)                                                        \
	({                                                                                            \
		typeof(map) curr =                                                                        \
				(typeof(map))_map_prepare((arena), (struct Map_Tmpl *)(map), (slice), sizeof((val))); \
		if (!curr->occupied) {                                                                    \
			curr->occupied = true;                                                                \
			curr->value = (val);                                                                  \
		}                                                                                         \
		curr;                                                                                     \
	})

AOCDEF void
_map_dump(struct Map_Tmpl *restrict m, rc *restrict buff, const int indent, const int depth);
AOCDEF struct Map_Tmpl *null _map_prepare(Arena *restrict arena,
									  struct Map_Tmpl *restrict map,
									  const Slice slice,
									  const size_t sizeof_value);
AOCDEF struct Map_Tmpl *null _map_get(struct Map_Tmpl *map, const Slice slice);
AOCDEF
int map_binary_search(struct Map_Tmpl *map, const unsigned char k);

#ifdef AOC_IMPLEMENTATION
#include "map.c"
#endif

#endif
