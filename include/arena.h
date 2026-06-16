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

#define AOCLIBS_ARENA_BACKEND_LIBC_MALLOC 0
#define AOCLIBS_ARENA_BACKEND_VIRTUAL_ALLOC 1

#ifndef AOCLIBS_ARENA_BACKEND
#define AOCLIBS_ARENA_BACKEND AOCLIBS_ARENA_BACKEND_LIBC_MALLOC
#endif // AOCLIBS_ARENA_BACKEND

typedef struct Region Region;

struct Region {
	Region *null next;
	size_t len;
	size_t cap;
	uintptr_t data[];
};

typedef struct {
	Region *null begin, *null end;
} Arena;

#ifndef AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY
#define AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY (8 * 1024)
#endif // AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY

// Allocates a new region in the heap with given "capacity".
//
// This memory is freed using "arena_free_region".
//
// NULL :: failed to allocate
AOCDEF Region *null arena_new_region(const size_t capacity);
AOCDEF void arena_free_region(Region *r);

// Reserve space from the arena with given "size_bytes".
//
// NULL :: failed to allocate << arena_new_region
AOCDEF void *null arena_alloc(Arena *a, const size_t size_bytes);

// Reserve space from the arena, and zero initialize with given "size_bytes".
//
// NULL :: failed to allocate << arena_new_region
AOCDEF void *null arena_calloc(Arena *a, const size_t size_bytes);

// Reserve new space from the arena with given "newsz".
//
// NULL :: failed to allocate << arena_new_region
AOCDEF void *null arena_realloc(Arena *restrict a,
								void *null restrict oldptr,
								const size_t oldsz,
								const size_t newsz);

// Reserve space from the arena, and copies "data" into it. The user must also provide its "size".
//
// NULL :: failed to allocate << arena_new_region
AOCDEF void *null arena_memdup(Arena *restrict a, void *restrict data, const size_t size);

// Reserve space from the arena, and copies formatted string with given "format".
//
// This serves as higher abstraction to arena_vsprintf.
//
// NULL :: failed to allocate << arena_new_region
AOCDEF char *null arena_sprintf(Arena *restrict a, const char *restrict format, ...);
AOCDEF char *null arena_vsprintf(Arena *restrict a, const char *restrict format, va_list args);

// Resets all contents, without freeing memory. Allows reuse.
AOCDEF void arena_reset(Arena *a);

// Frees the Arena memory. Doesn't allow reuse.
AOCDEF void arena_destroy(Arena *a);

// Frees the Arena memory. Allows reuse.
AOCDEF void arena_trim(Arena *a);

/*
 * Dynamic Arena
*/

#ifndef AOCLIBS_ARENA_DA_CAPACITY
#define AOCLIBS_ARENA_DA_CAPACITY 256
#endif

#define dar_reserve(a, da, new_cap) _dar_reserve(a, da, new_cap, sizeof(*(da)->data))

#define _dar_reserve(a, da, new_cap, sizeof_da)                                                  \
	do {                                                                                         \
		if (UNLIKELY((da)->len >= (da)->cap)) {                                                  \
			size_t new_capacity =                                                                \
					(da)->cap < AOCLIBS_ARENA_DA_CAPACITY ? AOCLIBS_ARENA_DA_CAPACITY : new_cap; \
			while ((new_cap) > new_capacity) {                                                   \
				new_capacity *= 2;                                                               \
			}                                                                                    \
			(da)->data = (__typeof__((da)->data))arena_realloc(                                  \
					(a), (da)->data, (da)->cap * (sizeof_da), new_capacity * (sizeof_da));       \
			ASSERT((da)->data, "out of memory while reserving memory for arena dynamic array");  \
			(da)->cap = new_capacity;                                                            \
		}                                                                                        \
	} while (0)

#define dar_insert(a, da, item)            \
	do {                                   \
		dar_reserve(a, da, (da)->len + 1); \
		(da)->data[(da)->len++] = (item);  \
	} while (0)

#define dar_append(a, da, items_buff, items_size) dar_add(a, da, items_buff, items_size, (da)->len)

#define dar_add(a, da, items_buff, items_size, offset)                                   \
	do {                                                                                 \
		dar_reserve(a, da, (da)->len + (items_size));                                    \
		memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
		(da)->len += (items_size);                                                       \
	} while (0)

#define dar_add_null(a, da) dar_append(a, da, "\0", 1)

#define arc_lcat(a, rc, cstr) dar_append(a, rc, cstr, STRLEN(cstr))
#define arc_cat(a, rc, cstr, len) dar_append(a, rc, cstr, len)

#define arc_lappend(a, rc, items_buff) arc_append(a, rc, items_buff, STRLEN(items_buff))
#define arc_append(a, rc, items_buff, items_size)                                         \
	do {                                                                                  \
		dar_reserve(a, (rc), 1 + (rc)->len + (items_size));                               \
		(rc)->data[(rc)->len++] = ' ';                                                    \
		memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
		(rc)->len += (items_size);                                                        \
	} while (0)

#define arc_appendf(a, rc, fmt, ...)                                                             \
	do {                                                                                         \
		const int needed = cstr_fmt_size(fmt, __VA_ARGS__);                                      \
		dar_reserve(a, rc, (rc)->len + needed);                                                  \
		const int written =                                                                      \
				cstr_fmt_write((rc)->data + (rc)->len, (rc)->cap - (rc)->len, fmt, __VA_ARGS__); \
		(rc)->len += written;                                                                    \
	} while (0)

// =================================
// These macros skips the reserve step, for faster operations with less checks.
// Use at own risk.
#define dar_insert_fast(a, da, item)      \
	do {                                  \
		(da)->data[(da)->len++] = (item); \
	} while (0)

#define dar_append_fast(a, da, items_buff, items_size) \
	dar_add_fast(a, da, items_buff, items_size, (da)->len)

#define dar_add_fast(a, da, items_buff, items_size, offset)                              \
	do {                                                                                 \
		memcpy((da)->data + (offset), (items_buff), (items_size) * sizeof(*(da)->data)); \
		(da)->len += (items_size);                                                       \
	} while (0)

#define arc_lcat_fast(a, rc, cstr) dar_append_fast(a, rc, cstr, STRLEN(cstr))
#define arc_cat_fast(a, rc, cstr, len) dar_append_fast(a, rc, cstr, len)

#define arc_lappend_fast(a, rc, items_buff) arc_append(a, rc, items_buff, STRLEN(items_buff))
#define arc_append_fast(a, rc, items_buff, items_size)                                    \
	do {                                                                                  \
		(rc)->data[(rc)->len++] = ' ';                                                    \
		memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
		(rc)->len += (items_size);                                                        \
	} while (0)
// =================================

#ifdef AOCLIBS_IMPLEMENTATION
#include "arena.c"
#endif // AOCLIBS_IMPLEMENTATION

#endif // AOCLIBS_ARENA_H_
