#ifndef AOCLIBS_MAP_H_
#define AOCLIBS_MAP_H_

#include "base.h"
#include "slices.h"
#include "arena.h"

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

typedef struct Map {
    // Dynamic Array
    size_t cap;
    size_t len;
    struct Map *data;

    char key;
    void *value;
} Map;

AOCDEF void map_insert(Arena *arena, Map *m, Slice *s, void *key);
AOCDEF void map_dump(Map *m);
AOCDEF Map *null map_find(Map *m, Slice *s);
AOCDEF bool map_set(Map *m, Slice *s, void *value);
AOCDEF bool map_delete(Map *m, Slice *s);

// Internal
AOCDEF int _map_bs(Map *m, unsigned char k);
AOCDEF void _map_insert(Arena *arena, Map *m, Slice *s, void *key, size_t cursor);
AOCDEF void _map_dump(Map *m, int indent, rc *buff, int depth);
AOCDEF Map *null _map_find(Map *m, Slice *s, size_t cursor);

#ifdef AOCLIBS_IMPLEMENTATION
#include "map.c"
#endif

#endif
