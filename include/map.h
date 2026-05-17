#ifndef AOCLIBS_MAP_H_
#define AOCLIBS_MAP_H_

#include "base.h"
#include "cstr.h"
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

#define generate_header_map(TYPE, VALUE_TYPE, PREFIX)                              \
    TYPE *_##PREFIX##_prepare(Arena *arena, TYPE *m, Slice *s, size_t cursor);     \
    TYPE *PREFIX##_prepare(Arena *arena, TYPE *m, Slice *s);                       \
    AOCDEF TYPE *PREFIX##_insert(Arena *arena, TYPE *m, Slice *s, VALUE_TYPE key); \
    AOCDEF void PREFIX##_dump(TYPE *m);                                            \
    AOCDEF TYPE *null PREFIX##_find(TYPE *m, Slice *s);                            \
    AOCDEF bool PREFIX##_set(TYPE *m, Slice *s, VALUE_TYPE value);                 \
    AOCDEF bool PREFIX##_delete(TYPE *m, Slice *s);                                \
                                                                                   \
    AOCDEF int _##PREFIX##_bs(TYPE *m, unsigned char k);                           \
    AOCDEF TYPE *_##PREFIX##_insert(                                               \
            Arena *arena, TYPE *m, Slice *s, VALUE_TYPE key, size_t cursor);       \
    AOCDEF void _##PREFIX##_dump(TYPE *m, int indent, rc *buff, int depth);        \
    AOCDEF TYPE *null _##PREFIX##_find(TYPE *m, Slice *s, size_t cursor);

#define generate_definition_map(TYPE, VALUE_TYPE, PREFIX)                                      \
    int _##PREFIX##_bs(TYPE *m, unsigned char k) {                                             \
        int left = 0, right = m->len;                                                          \
                                                                                               \
        while (left < right) {                                                                 \
            int mid = left + (right - left) / 2;                                               \
            if ((unsigned char)m->data[mid].key < k) {                                         \
                left = mid + 1;                                                                \
            } else {                                                                           \
                right = mid;                                                                   \
            }                                                                                  \
        }                                                                                      \
                                                                                               \
        return left;                                                                           \
    }                                                                                          \
                                                                                               \
    TYPE *_##PREFIX##_prepare(Arena *arena, TYPE *m, Slice *s, size_t cursor) {                \
        if (cursor == s->len) {                                                                \
            return m;                                                                          \
        }                                                                                      \
                                                                                               \
        unsigned char k = (unsigned char)s->data[cursor];                                      \
                                                                                               \
        int pos = _##PREFIX##_bs(m, k);                                                        \
                                                                                               \
        if (pos < m->len && (unsigned char)m->data[pos].key == k) {                            \
            return _##PREFIX##_prepare(arena, &m->data[pos], s, cursor + 1);                   \
        }                                                                                      \
                                                                                               \
        int left = 0, right = m->len;                                                          \
        while (left < right) {                                                                 \
            int mid = left + (right - left) / 2;                                               \
            if ((unsigned char)m->data[mid].key < k)                                           \
                left = mid + 1;                                                                \
            else                                                                               \
                right = mid;                                                                   \
        }                                                                                      \
                                                                                               \
        dar_reserve(arena, m, m->len + 1);                                                     \
        memmove(&m->data[left + 1], &m->data[left], (m->len - left) * sizeof(TYPE));           \
                                                                                               \
        TYPE c = { 0 };                                                                        \
        c.key = k;                                                                             \
                                                                                               \
        m->data[left] = c;                                                                     \
        m->len++;                                                                              \
                                                                                               \
        return _##PREFIX##_prepare(arena, &m->data[left], s, cursor + 1);                      \
    }                                                                                          \
                                                                                               \
    TYPE *PREFIX##_prepare(Arena *arena, TYPE *m, Slice *s) {                                  \
        return _##PREFIX##_prepare(arena, m, s, 0);                                            \
    }                                                                                          \
                                                                                               \
    TYPE *_##PREFIX##_insert(Arena *arena, TYPE *m, Slice *s, VALUE_TYPE key, size_t cursor) { \
        if (cursor == s->len) {                                                                \
            if (m->occupied == true) return m;                                                 \
            m->occupied = true;                                                                \
            m->value = key;                                                                    \
            return m;                                                                          \
        }                                                                                      \
                                                                                               \
        unsigned char k = (unsigned char)s->data[cursor];                                      \
                                                                                               \
        int pos = _##PREFIX##_bs(m, k);                                                        \
                                                                                               \
        if (pos < m->len && (unsigned char)m->data[pos].key == k) {                            \
            return _##PREFIX##_insert(arena, &m->data[pos], s, key, cursor + 1);               \
        }                                                                                      \
                                                                                               \
        int left = 0, right = m->len;                                                          \
        while (left < right) {                                                                 \
            int mid = left + (right - left) / 2;                                               \
            if ((unsigned char)m->data[mid].key < k)                                           \
                left = mid + 1;                                                                \
            else                                                                               \
                right = mid;                                                                   \
        }                                                                                      \
                                                                                               \
        dar_reserve(arena, m, m->len + 1);                                                     \
        memmove(&m->data[left + 1], &m->data[left], (m->len - left) * sizeof(TYPE));           \
                                                                                               \
        TYPE c = { 0 };                                                                        \
        c.key = k;                                                                             \
                                                                                               \
        m->data[left] = c;                                                                     \
        m->len++;                                                                              \
                                                                                               \
        return _##PREFIX##_insert(arena, &m->data[left], s, key, cursor + 1);                  \
    }                                                                                          \
                                                                                               \
    TYPE *PREFIX##_insert(Arena *arena, TYPE *m, Slice *s, VALUE_TYPE key) {                   \
        return _##PREFIX##_insert(arena, m, s, key, 0);                                        \
    }                                                                                          \
                                                                                               \
    void _##PREFIX##_dump(TYPE *m, int indent, rc *buff, int depth) {                          \
        da_reserve(buff, depth + 1);                                                           \
        range(0, m->len, i) {                                                                  \
            buff->data[depth] = m->data[i].key;                                                \
            buff->data[depth + 1] = '\0';                                                      \
                                                                                               \
            fputn(stderr, indent, ' ');                                                        \
            fprintf(stderr, "%c -> %s\n", m->data[i].key, buff->data);                         \
                                                                                               \
            _##PREFIX##_dump(&m->data[i], indent + 3, buff, depth + 1);                        \
        }                                                                                      \
    }                                                                                          \
                                                                                               \
    void PREFIX##_dump(TYPE *m) {                                                              \
        rc buff = { 0 };                                                                       \
        _##PREFIX##_dump(m, 0, &buff, 0);                                                      \
        da_free(&buff);                                                                        \
    }                                                                                          \
                                                                                               \
    TYPE *null _##PREFIX##_find(TYPE *m, Slice *s, size_t cursor) {                            \
        if (cursor == s->len) return m;                                                        \
        unsigned char k = (unsigned char)s->data[cursor];                                      \
                                                                                               \
        int pos = _##PREFIX##_bs(m, k);                                                        \
                                                                                               \
        if (pos < m->len && (unsigned char)m->data[pos].key == k) {                            \
            return _##PREFIX##_find(&m->data[pos], s, cursor + 1);                             \
        }                                                                                      \
        return NULL;                                                                           \
    }                                                                                          \
                                                                                               \
    TYPE *null PREFIX##_find(TYPE *m, Slice *s) {                                              \
        TYPE *tmp = _##PREFIX##_find(m, s, 0);                                                 \
        return (tmp && tmp->occupied) ? tmp : NULL;                                            \
    }                                                                                          \
                                                                                               \
    bool PREFIX##_set(TYPE *m, Slice *s, VALUE_TYPE value) {                                   \
        TYPE *c = PREFIX##_find(m, s);                                                         \
        if (!c) return false;                                                                  \
                                                                                               \
        c->value = value;                                                                      \
        return true;                                                                           \
    }                                                                                          \
                                                                                               \
    bool PREFIX##_delete(TYPE *m, Slice *s) {                                                  \
        TYPE *c = PREFIX##_find(m, s);                                                         \
        if (!c) return false;                                                                  \
        c->occupied = false;                                                                   \
        return true;                                                                           \
    }

#endif
