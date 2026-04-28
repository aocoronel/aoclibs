#ifndef AOCLIBS_MAP_H_
#define AOCLIBS_MAP_H_

#include "base.h"
#include "slices.h"
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

// Example:
// typedef struct Map {
//     // Dynamic Array
//     size_t cap;
//     size_t len;
//     struct Map *data;
//
//     char key;
//     void *value;
// } Map;

#define generate_header_map(TYPE, PREFIX)                                                      \
    AOCDEF void PREFIX##_insert(Arena *arena, TYPE *m, Slice *s, void *key);                   \
    AOCDEF void PREFIX##_dump(TYPE *m);                                                        \
    AOCDEF TYPE *null PREFIX##_find(TYPE *m, Slice *s);                                        \
    AOCDEF bool PREFIX##_set(TYPE *m, Slice *s, void *value);                                  \
    AOCDEF bool PREFIX##_delete(TYPE *m, Slice *s);                                            \
                                                                                               \
    AOCDEF int _##PREFIX##_bs(TYPE *m, unsigned char k);                                       \
    AOCDEF void _##PREFIX##_insert(Arena *arena, TYPE *m, Slice *s, void *key, size_t cursor); \
    AOCDEF void _##PREFIX##_dump(TYPE *m, int indent, rc *buff, int depth);                    \
    AOCDEF TYPE *null _##PREFIX##_find(TYPE *m, Slice *s, size_t cursor);

#define generate_definition_map(TYPE, PREFIX)                                            \
    int _##PREFIX##_bs(TYPE *m, unsigned char k) {                                       \
        int left = 0, right = m->len;                                                    \
                                                                                         \
        while (left < right) {                                                           \
            int mid = left + (right - left) / 2;                                         \
            if ((unsigned char)m->data[mid].key < k) {                                   \
                left = mid + 1;                                                          \
            } else {                                                                     \
                right = mid;                                                             \
            }                                                                            \
        }                                                                                \
                                                                                         \
        return left;                                                                     \
    }                                                                                    \
                                                                                         \
    void _##PREFIX##_insert(Arena *arena, TYPE *m, Slice *s, void *key, size_t cursor) { \
        if (cursor == s->len) {                                                          \
            m->value = key;                                                              \
            return;                                                                      \
        }                                                                                \
                                                                                         \
        unsigned char k = (unsigned char)s->data[cursor];                                \
                                                                                         \
        int pos = _##PREFIX##_bs(m, k);                                                  \
                                                                                         \
        if (pos < m->len && (unsigned char)m->data[pos].key == k) {                      \
            return _##PREFIX##_insert(arena, &m->data[pos], s, key, cursor + 1);         \
        }                                                                                \
                                                                                         \
        int left = 0, right = m->len;                                                    \
        while (left < right) {                                                           \
            int mid = left + (right - left) / 2;                                         \
            if ((unsigned char)m->data[mid].key < k)                                     \
                left = mid + 1;                                                          \
            else                                                                         \
                right = mid;                                                             \
        }                                                                                \
                                                                                         \
        dar_reserve(arena, m, m->len + 1);                                               \
        memmove(&m->data[left + 1], &m->data[left], (m->len - left) * sizeof(TYPE));     \
                                                                                         \
        TYPE c = { 0 };                                                                  \
        c.key = k;                                                                       \
                                                                                         \
        m->data[left] = c;                                                               \
        m->len++;                                                                        \
                                                                                         \
        _##PREFIX##_insert(arena, &m->data[left], s, key, cursor + 1);                   \
    }                                                                                    \
                                                                                         \
    void PREFIX##_insert(Arena *arena, TYPE *m, Slice *s, void *key) {                   \
        _##PREFIX##_insert(arena, m, s, key, 0);                                         \
    }                                                                                    \
                                                                                         \
    void _##PREFIX##_dump(TYPE *m, int indent, rc *buff, int depth) {                    \
        da_reserve(buff, depth + 1);                                                     \
        range(0, m->len, i) {                                                            \
            buff->data[depth] = m->data[i].key;                                          \
            buff->data[depth + 1] = '\0';                                                \
                                                                                         \
            fputn(stderr, indent, ' ');                                                  \
            fprintf(stderr, "%c -> %s\n", m->data[i].key, buff->data);                   \
                                                                                         \
            _##PREFIX##_dump(&m->data[i], indent + 3, buff, depth + 1);                  \
        }                                                                                \
    }                                                                                    \
                                                                                         \
    void PREFIX##_dump(TYPE *m) {                                                        \
        rc buff = { 0 };                                                                 \
        _##PREFIX##_dump(m, 0, &buff, 0);                                                \
        da_free(&buff);                                                                  \
    }                                                                                    \
                                                                                         \
    TYPE *null _##PREFIX##_find(TYPE *m, Slice *s, size_t cursor) {                      \
        if (cursor == s->len) return m;                                                  \
        unsigned char k = (unsigned char)s->data[cursor];                                \
                                                                                         \
        int pos = _##PREFIX##_bs(m, k);                                                  \
                                                                                         \
        if (pos < m->len && (unsigned char)m->data[pos].key == k) {                      \
            return _##PREFIX##_find(&m->data[pos], s, cursor + 1);                       \
        }                                                                                \
        return NULL;                                                                     \
    }                                                                                    \
                                                                                         \
    TYPE *null PREFIX##_find(TYPE *m, Slice *s) {                                        \
        TYPE *tmp = _##PREFIX##_find(m, s, 0);                                           \
        return (tmp && tmp->value) ? tmp : NULL;                                         \
    }                                                                                    \
                                                                                         \
    bool PREFIX##_set(TYPE *m, Slice *s, void *value) {                                  \
        TYPE *c = PREFIX##_find(m, s);                                                   \
        if (!c) return false;                                                            \
                                                                                         \
        c->value = value;                                                                \
        return true;                                                                     \
    }                                                                                    \
                                                                                         \
    bool PREFIX##_delete(TYPE *m, Slice *s) {                                            \
        TYPE *c = PREFIX##_find(m, s);                                                   \
        if (!c) return false;                                                            \
                                                                                         \
        c->value = NULL;                                                                 \
        return true;                                                                     \
    }

#endif
