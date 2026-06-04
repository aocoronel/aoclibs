#ifndef AOCLIBS_MAP_H_
#define AOCLIBS_MAP_H_

#include "base.h"
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
struct _Map {
    size_t len;
    size_t cap;
    struct _Map *data;

    bool occupied;
    char key;
    int value;
};

#define map_dump(map, buff) _map_dump((struct _Map *)(map), (buff), 0, 0)

#define map_delete(map, slice)                       \
    ({                                               \
        bool result = true;                          \
        __typeof__(map) c = map_get((map), (slice)); \
        if (!c) {                                    \
            result = false;                          \
        } else {                                     \
            (c)->occupied = false;                   \
        }                                            \
        result;                                      \
    })

#define map_set(map, slice, val)                     \
    ({                                               \
        bool result = true;                          \
        __typeof__(map) c = map_get((map), (slice)); \
        if (!c) {                                    \
            result = false;                          \
        } else {                                     \
            c->value = (val);                        \
        }                                            \
        result;                                      \
    })

#define map_get(map, slice) (__typeof__(map))_map_get((struct _Map *)(map), (slice))

#define map_prepare(arena, map, slice) \
    (__typeof__(map))_map_prepare((arena), (struct _Map *)(map), (slice), sizeof((val)))

#define map_insert(arena, map, slice, val)                              \
    ({                                                                  \
        __typeof__(map) curr = (__typeof__(map))_map_prepare(           \
                (arena), (struct _Map *)(map), (slice), sizeof((val))); \
        if (!curr->occupied) {                                          \
            curr->occupied = true;                                      \
            curr->value = (val);                                        \
        }                                                               \
        curr;                                                           \
    })

AOCDEF void
_map_dump(struct _Map *restrict m, rc *restrict buff, const int indent, const int depth);
AOCDEF struct _Map *null _map_prepare(Arena *restrict arena,
                                      struct _Map *restrict map,
                                      const Slice slice,
                                      const size_t sizeof_value);
AOCDEF struct _Map *null _map_get(struct _Map *map, const Slice slice);
AOCDEF
int map_binary_search(struct _Map *map, const unsigned char k);

#ifdef AOCLIBS_IMPLEMENTATION
#include "map.c"
#endif

#endif
