#ifndef AOCLIBS_ARENA_H_
#define AOCLIBS_ARENA_H_

// Copyright 2022 Alexey Kutepov <reximkut@gmail.com>

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "base.h"
#include <stdint.h>
#include <stdarg.h>

#define AOCLIBS_ARENA_BACKEND_LIBC_MALLOC 0
#define AOCLIBS_ARENA_BACKEND_VIRTUAL_ALLOC 1

#ifndef AOCLIBS_ARENA_BACKEND
#define AOCLIBS_ARENA_BACKEND AOCLIBS_ARENA_BACKEND_LIBC_MALLOC
#endif // AOCLIBS_ARENA_BACKEND

typedef struct Region Region;

struct Region {
        Region *next;
        size_t len;
        size_t cap;
        uintptr_t data[];
};

typedef struct {
        Region *begin, *end;
} Arena;

#ifndef AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY
#define AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY (8 * 1024)
#endif // AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY

// Allocates a new region in the heap with given "capacity".
//
// This memory is freed using "arena_free_region".
//
// NULL :: failed to allocate
AOCLIBS_PREFIX Region *null aoc_arena_new_region(size_t capacity);
AOCLIBS_PREFIX void aoc_arena_free_region(Region *ref r);

// Reserve space from the arena with given "size_bytes".
//
// NULL :: failed to allocate << arena_new_region
AOCLIBS_PREFIX void *aoc_arena_alloc(Arena *ref a, size_t size_bytes);

// Reserve space from the arena, and zero initialize with given "size_bytes".
//
// NULL :: failed to allocate << arena_new_region
AOCLIBS_PREFIX void *aoc_arena_calloc(Arena *ref a, size_t size_bytes);

// Reserve new space from the arena with given "newsz".
//
// NULL :: failed to allocate << arena_new_region
AOCLIBS_PREFIX void *aoc_arena_realloc(Arena *ref a, void *ref oldptr, size_t oldsz, size_t newsz);

// Reserve space from the arena, and copies "data" into it. The user must also provide its "size".
//
// NULL :: failed to allocate << arena_new_region
AOCLIBS_PREFIX void *aoc_arena_memdup(Arena *ref a, void *ref data, size_t size);

// Reserve space from the arena, and copies formatted string with given "format".
//
// This serves as higher abstraction to arena_vsprintf.
//
// NULL :: failed to allocate << arena_new_region
AOCLIBS_PREFIX char *aoc_arena_sprintf(Arena *ref a, const char *format, ...);
AOCLIBS_PREFIX char *aoc_arena_vsprintf(Arena *ref a, const char *format, va_list args);

// Resets all contents, without freeing memory. Allows reuse.
AOCLIBS_PREFIX void aoc_arena_reset(Arena *ref a);

// Frees the Arena memory. Doesn't allow reuse.
AOCLIBS_PREFIX void aoc_arena_destroy(Arena *ref a);

// Frees the Arena memory. Allows reuse.
AOCLIBS_PREFIX void aoc_arena_trim(Arena *ref a);

/*
 * Dynamic Arena
*/

#ifndef AOCLIBS_ARENA_DA_CAPACITY
#define AOCLIBS_ARENA_DA_CAPACITY 256
#endif

#define aoc_dar_reserve(a, da, new_cap)                                                     \
        do {                                                                                \
                if ((da)->len >= (da)->cap) {                                               \
                        size_t new_capacity = (da)->cap < AOCLIBS_ARENA_DA_CAPACITY ?       \
                                                      AOCLIBS_ARENA_DA_CAPACITY :           \
                                                      new_cap;                              \
                        while ((new_cap) > new_capacity) {                                  \
                                new_capacity *= 2;                                          \
                        }                                                                   \
                        (da)->data = aoc_arena_realloc((a),                                 \
                                                       (da)->data,                          \
                                                       (da)->cap * sizeof(*(da)->data),     \
                                                       new_capacity * sizeof(*(da)->data)); \
                        (da)->cap = new_capacity;                                           \
                }                                                                           \
        } while (0)

#define aoc_dar_insert(a, da, item)                    \
        do {                                           \
                aoc_dar_reserve(a, da, (da)->len + 1); \
                (da)->data[(da)->len++] = (item);      \
        } while (0)

#define aoc_dar_copy(a, da, items_buff, items_size) \
        aoc_dar_add(a, da, items_buff, items_size, (da)->len)

#define aoc_dar_add(a, da, items_buff, items_size, offset)                                       \
        do {                                                                                     \
                aoc_dar_reserve(a, da, (da)->len + (items_size));                                \
                memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
                (da)->len += (items_size);                                                       \
        } while (0)

#define aoc_dar_add_null(a, da) aoc_dar_copy(a, da, "\0", 1)

#define aoc_arcl_cat(a, rc, cstr) aoc_dar_copy(a, rc, cstr, STRLEN(cstr))
#define aoc_arc_cat(a, rc, cstr, len) aoc_dar_copy(a, rc, cstr, len)

#define aoc_arcl_append(a, rc, items_buff) aoc_arc_append(a, rc, items_buff, STRLEN(items_buff))
#define aoc_arc_append(a, rc, items_buff, items_size)                                             \
        do {                                                                                      \
                aoc_dar_reserve(a, (rc), 1 + (rc)->len + (items_size));                           \
                (rc)->data[(rc)->len++] = ' ';                                                    \
                memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
                (rc)->len += (items_size);                                                        \
        } while (0)

#define aoc_arc_fmt_append(a, rc, fmt, ...)                                               \
        do {                                                                              \
                int needed = aoc_cstr_fmt_size(fmt, __VA_ARGS__);                         \
                aoc_dar_reserve(a, rc, (rc)->len + needed);                               \
                int written = aoc_cstr_fmt_write(                                         \
                        (rc)->data + (rc)->len, (rc)->cap - (rc)->len, fmt, __VA_ARGS__); \
                (rc)->len += written;                                                     \
        } while (0)

// =================================
// These macros skips the reserve step, for faster operations with less checks.
// Use at own risk.
#define aoc_dar_insert_fast(a, da, item)          \
        do {                                      \
                (da)->data[(da)->len++] = (item); \
        } while (0)

#define aoc_dar_copy_fast(a, da, items_buff, items_size) \
        aoc_dar_add_fast(a, da, items_buff, items_size, (da)->len)

#define aoc_dar_add_fast(a, da, items_buff, items_size, offset)                                  \
        do {                                                                                     \
                memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
                (da)->len += (items_size);                                                       \
        } while (0)

#define aoc_arcl_cat_fast(a, rc, cstr) aoc_dar_copy_fast(a, rc, cstr, STRLEN(cstr))
#define aoc_arc_cat_fast(a, rc, cstr, len) aoc_dar_copy_fast(a, rc, cstr, len)

#define aoc_arcl_append_fast(a, rc, items_buff) \
        aoc_arc_append(a, rc, items_buff, STRLEN(items_buff))
#define aoc_arc_append_fast(a, rc, items_buff, items_size)                                        \
        do {                                                                                      \
                (rc)->data[(rc)->len++] = ' ';                                                    \
                memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
                (rc)->len += (items_size);                                                        \
        } while (0)
// =================================

#ifdef AOCLIBS_IMPLEMENTATION
#include "arena.c"
#endif // AOCLIBS_IMPLEMENTATION

#ifdef AOCLIBS_STRIP_PREFIX
#define arena_new_region aoc_arena_new_region
#define arena_free_region aoc_arena_free_region

#define arena_alloc aoc_arena_alloc
#define arena_realloc aoc_arena_realloc
#define arena_memdup aoc_arena_memdup
#define arena_sprintf aoc_arena_sprintf
#define arena_vsprintf aoc_arena_vsprintf

#define arena_reset aoc_arena_reset
#define arena_destroy aoc_arena_destroy
#define arena_trim aoc_arena_trim

#define dar_add_null aoc_dar_add_null
#define dar_copy aoc_dar_copy
#define dar_insert aoc_dar_insert
#define dar_reserve aoc_dar_reserve
#define arcl_copy aoc_arcl_copy

#define dar_reserve aoc_dar_reserve
#define dar_insert aoc_dar_insert
#define dar_copy aoc_dar_copy
#define dar_add aoc_dar_add
#define dar_add_null aoc_dar_add_null

#define arcl_cat aoc_arcl_cat
#define arc_cat aoc_arc_cat

#define arcl_append aoc_arcl_append
#define arc_append aoc_arc_append

#define arc_fmt_append aoc_arc_fmt_append

#define dar_add_fast aoc_dar_add_fast
#define dar_copy_fast aoc_dar_copy_fast
#define dar_insert_fast aoc_dar_insert_fast

#define arc_cat_fast aoc_arc_cat_fast
#define arc_append_fast aoc_arc_append_fast
#endif

#endif // AOCLIBS_ARENA_H_
