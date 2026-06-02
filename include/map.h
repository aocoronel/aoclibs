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

#define map_delete(map, slice)                        \
    ({                                                \
        bool result = true;                           \
        __typeof__(map) c = map_find((map), (slice)); \
        if (!c) {                                     \
            result = false;                           \
        } else {                                      \
            (c)->occupied = false;                    \
        }                                             \
        result;                                       \
    })

#define map_set(map, slice, val)                      \
    ({                                                \
        bool result = true;                           \
        __typeof__(map) c = map_find((map), (slice)); \
        if (!c) {                                     \
            result = false;                           \
        } else {                                      \
            c->value = (val);                         \
        }                                             \
        result;                                       \
    })

#define map_find(map, slice) (__typeof__(map))_map_find((struct _Map *)(map), (slice))

#define map_prepare(arena, map, slice) \
    (__typeof__(map))_map_prepare((arena), (struct _Map *)(map), (slice))

#define map_insert(arena, map, slice, val)                                             \
    ({                                                                                 \
        __typeof__(map) curr =                                                         \
                (__typeof__(map))_map_prepare((arena), (struct _Map *)(map), (slice)); \
        void *ptr = _map_prepare((arena), (struct _Map *)(map), (slice));              \
        if (!curr->occupied) {                                                         \
            curr->occupied = true;                                                     \
            curr->value = (val);                                                       \
        }                                                                              \
        curr;                                                                          \
    })

AOCDEF void _map_dump(struct _Map *restrict m, rc *restrict buff, const int indent, const int depth);
AOCDEF struct _Map *null _map_prepare(Arena *restrict arena, struct _Map *restrict map, const Slice slice);
AOCDEF struct _Map *null _map_find(struct _Map *map, const Slice slice);
AOCDEF
int map_binary_search(struct _Map *map, const unsigned char k);

#ifdef AOCLIBS_IMPLEMENTATION
int map_binary_search(struct _Map *map, const unsigned char k) {
    ASSERT_NONNULL(map);
    int left = 0, right = (int)map->len;
    while (left < right) {
        int mid = left + (right - left) / 2;
        if ((unsigned char)map->data[mid].key < k) {
            left = mid + 1;
        } else {
            right = mid;
        }
    }
    return left;
}

void _map_dump(struct _Map *restrict m, rc *restrict buff, const int indent, const int depth) {
    ASSERT_NONNULL(m);
    ASSERT_NONNULL(buff);
    da_reserve(buff, depth + 1);
    range(0, m->len, i) {
        buff->data[depth] = m->data[i].key;
        buff->data[depth + 1] = '\0';

        fputn(stderr, indent, ' ');
        fprintf(stderr, "%c -> %s\n", m->data[i].key, buff->data);

        _map_dump(&m->data[i], buff, indent + 3, depth + 1);
    }
}

struct _Map *_map_prepare(Arena *restrict arena, struct _Map *restrict map, const Slice slice) {
    ASSERT_NONNULL(arena);
    ASSERT_NONNULL(map);
    struct _Map *curr = map;
    size_t cursor = 0;

    while (cursor < (slice).len) {
        unsigned char k = (unsigned char)(slice).data[cursor];

        int pos = map_binary_search(curr, k);

        if (pos < (int)curr->len && (unsigned char)curr->data[pos].key == k) {
            curr = &curr->data[pos];
            cursor++;
            continue;
        }

        dar_reserve(arena, curr, curr->len + 1);

        memmove(&curr->data[pos + 1], &curr->data[pos], (curr->len - pos) * sizeof(curr->data[0]));

        struct _Map next = { 0 };
        next.key = k;

        curr->data[pos] = next;
        curr->len++;

        curr = &curr->data[pos];
        cursor++;
    }
    return curr;
}

struct _Map *_map_find(struct _Map *map, const Slice slice) {
    ASSERT_NONNULL(map);
    struct _Map *result = map;
    size_t cursor = 0;

    while (result && cursor < (slice).len) {
        unsigned char k = (unsigned char)(slice).data[cursor];

        int pos = map_binary_search(result, k);

        if (pos < (int)result->len && (unsigned char)result->data[pos].key == k) {
            result = &result->data[pos];
            cursor++;
        } else {
            result = NULL;
        }
    }

    return (result && result->occupied) ? result : NULL;
}

#endif

// typedef struct Map {
//     size_t len;
//     size_t cap;
//     struct Map *data;
//
//     bool occupied;
//     char key;
//     int value;
// } Map;
//
// int main(int argc, char *argv[]) {
//     Map map = { 0 };
//     Slice s = slice("hello");
//     Arena arena = { 0 };
//
//     map_insert(&arena, &map, slice("hello"), 20);
//     Map *m = map_find(&map, slice("hello"));
//     printf("Value: %d\n", m->value);
//
//     map_set(&map, s, 23);
//     printf("Value: %d\n", m->value);
//
//     Map *m2 = map_prepare(&arena, &map, slice("hello2"));
//     ASSERT(m2->occupied == false);
//
//     rc buff = { 0 };
//     map_dump(&map, &buff);
//     if (!map_delete(&map, s)) {
//         printf("Failed to delete!\n");
//     }
//     return 0;
// }

#endif
