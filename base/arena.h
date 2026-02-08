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

#ifndef ARENA_H_
#define ARENA_H_

#include "base.h"
#include <stddef.h>
#include <stdint.h>

#ifndef AOCLIBS_ARENA_NOSTDIO
#include <stdarg.h>
#include <stdio.h>
#endif // AOCLIBS_ARENA_NOSTDIO

#define AOCLIBS_ARENA_BACKEND_LIBC_MALLOC 0
#define AOCLIBS_ARENA_BACKEND_LINUX_MMAP 1
#define AOCLIBS_ARENA_BACKEND_WIN32_VIRTUALALLOC 2

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

AOCLIBS_PREFIX Region *aoc_arena_new_region(size_t capacity);
AOCLIBS_PREFIX void aoc_arena_free_region(Region *ref r);

AOCLIBS_PREFIX void *aoc_arena_alloc(Arena *ref a, size_t size_bytes);
AOCLIBS_PREFIX void *aoc_arena_realloc(Arena *ref a, void *ref oldptr, size_t oldsz, size_t newsz);
AOCLIBS_PREFIX void *aoc_arena_memdup(Arena *ref a, void *ref data, size_t size);
#ifndef AOCLIBS_ARENA_NOSTDIO
AOCLIBS_PREFIX char *aoc_arena_sprintf(Arena *ref a, const char *format, ...);
AOCLIBS_PREFIX char *aoc_arena_vsprintf(Arena *ref a, const char *format, va_list args);
#endif // AOCLIBS_ARENA_NOSTDIO

AOCLIBS_PREFIX void aoc_arena_reset(Arena *ref a);
AOCLIBS_PREFIX void aoc_arena_destroy(Arena *ref a);
AOCLIBS_PREFIX void aoc_arena_trim(Arena *ref a);

#ifndef AOCLIBS_ARENA_DA_INIT_CAP
#define AOCLIBS_ARENA_DA_INIT_CAP 4 * 1024
#endif // AOCLIBS_ARENA_DA_INIT_CAP

#define aoc_arena_da_append(a, da, item)                                                    \
        do {                                                                                \
                if ((da)->len >= (da)->cap) {                                               \
                        size_t new_capacity = (da)->cap == 0 ? AOCLIBS_ARENA_DA_INIT_CAP :  \
                                                               (da)->cap * 2;               \
                        (da)->data = aoc_arena_realloc((a), (da)->data,                     \
                                                       (da)->cap * sizeof(*(da)->data),     \
                                                       new_capacity * sizeof(*(da)->data)); \
                        (da)->cap = new_capacity;                                           \
                }                                                                           \
                                                                                            \
                (da)->data[(da)->len++] = (item);                                           \
        } while (0)

// Append several items to a dynamic array
#define aoc_arena_da_append_many(a, da, new_items, new_items_count)                         \
        do {                                                                                \
                if ((da)->len + (new_items_count) > (da)->cap) {                            \
                        size_t new_capacity = (da)->cap;                                    \
                        if (new_capacity == 0) new_capacity = AOCLIBS_ARENA_DA_INIT_CAP;    \
                        while ((da)->len + (new_items_count) > new_capacity)                \
                                new_capacity *= 2;                                          \
                        (da)->data = aoc_arena_realloc((a), (da)->data,                     \
                                                       (da)->cap * sizeof(*(da)->data),     \
                                                       new_capacity * sizeof(*(da)->data)); \
                        (da)->cap = new_capacity;                                           \
                }                                                                           \
                memcpy((da)->data + (da)->len, (new_items),                                 \
                       (new_items_count) * sizeof(*(da)->data));                            \
                (da)->len += (new_items_count);                                             \
        } while (0)

#define aoc_arena_da_append_null(a, da) aoc_arena_da_append(a, da, 0)

// These require cstr.h
#define aoc_arena_da_append_cstr(a, da, cstr, cstr_cap) \
        do {                                            \
                const char *s = (cstr);                 \
                size_t n = cstrlen(cstr, cstr_cap);     \
                aoc_arena_da_append_many(a, da, s, n);  \
        } while (0)

#define aoc_arena_da_append_cstrl(a, da, cstr) \
        aoc_arena_da_append_cstr(a, da, cstr, cstrl_len(cstr))

#endif // AOCLIBS_ARENA_H_

#define AOCLIBS_IMPLEMENTATION
#ifdef AOCLIBS_IMPLEMENTATION

#include <string.h>
#include <assert.h>

#if AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_LIBC_MALLOC
#include <stdlib.h>

AOCLIBS_PREFIX Region *aoc_arena_new_region(size_t capacity) {
        size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;
        Region *r = (Region *)malloc(size_bytes);
        assert(r && "Out of memory");
        r->next = NULL;
        r->len = 0;
        r->cap = capacity;
        return r;
}

AOCLIBS_PREFIX void aoc_arena_free_region(Region *ref r) {
        ASSERT_NONNULL(r != NULL);
        free(r);
}
#elif AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_LINUX_MMAP
#if !defined(__linux__)
#error "Current platform is not Linux"
#endif

#include <unistd.h>
#include <sys/mman.h>

AOCLIBS_PREFIX Region *aoc_arena_new_region(size_t capacity) {
        size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;
        Region *r =
                mmap(NULL, size_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
        assert(r != MAP_FAILED && "Failed to allocate virtual memory");
        r->next = NULL;
        r->len = 0;
        r->cap = capacity;
        return r;
}

AOCLIBS_PREFIX void aoc_arena_free_region(Region *r) {
        size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * r->cap;
        int ret = munmap(r, size_bytes);
        assert(ret == 0);
}

#elif AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_WIN32_VIRTUALALLOC

#if !defined(_WIN32)
#error "Current platform is not Windows"
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define INV_HANDLE(x) (((x) == NULL) || ((x) == INVALID_HANDLE_VALUE))

AOCLIBS_PREFIX Region *aoc_arena_new_region(size_t capacity) {
        SIZE_T size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;
        Region *r =
                VirtualAllocEx(GetCurrentProcess(), /* Allocate in current process address space */
                               NULL, /* Unknown position */
                               size_bytes, /* Bytes to allocate */
                               MEM_COMMIT | MEM_RESERVE, /* Reserve and commit allocated page */
                               PAGE_READWRITE /* Permissions ( Read/Write )*/
                );
        if (INV_HANDLE(r)) assert(0 && "VirtualAllocEx() failed.");

        r->next = NULL;
        r->len = 0;
        r->cap = capacity;
        return r;
}

AOCLIBS_PREFIX void aoc_arena_free_region(Region *r) {
        if (INV_HANDLE(r)) return;

        BOOL free_result = VirtualFreeEx(
                GetCurrentProcess(), /* Deallocate from current process address space */
                (LPVOID)r, /* Address to deallocate */
                0, /* Bytes to deallocate ( Unknown, deallocate entire page ) */
                MEM_RELEASE /* Release the page ( And implicitly decommit it ) */
        );

        if (FALSE == free_result) assert(0 && "VirtualFreeEx() failed.");
}
#else
#error "Unknown Arena backend"
#endif

AOCLIBS_PREFIX void *aoc_arena_alloc(Arena *ref a, size_t size_bytes) {
        ASSERT_NONNULL(a != NULL);
        size_t size = (size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

        if (a->end == NULL) {
                assert(a->begin == NULL);
                size_t capacity = AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY;
                if (capacity < size) capacity = size;
                a->end = aoc_arena_new_region(capacity);
                a->begin = a->end;
        }

        while (a->end->len + size > a->end->cap && a->end->next != NULL) {
                a->end = a->end->next;
        }

        if (a->end->len + size > a->end->cap) {
                assert(a->end->next == NULL);
                size_t capacity = AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY;
                if (capacity < size) capacity = size;
                a->end->next = aoc_arena_new_region(capacity);
                a->end = a->end->next;
        }

        void *result = &a->end->data[a->end->len];
        a->end->len += size;
        return result;
}

AOCLIBS_PREFIX void *aoc_arena_realloc(Arena *ref a, void *oldptr, size_t oldsz, size_t newsz) {
        ASSERT_NONNULL(a != NULL);
        ASSERT_NONNULL(oldptr != NULL);
        if (newsz <= oldsz) return oldptr;
        void *newptr = aoc_arena_alloc(a, newsz);
        char *newptr_char = (char *)newptr;
        char *oldptr_char = (char *)oldptr;
        memcpy(newptr_char, oldptr_char, oldsz);
        return newptr;
}

AOCLIBS_PREFIX void *aoc_arena_memdup(Arena *ref a, void *ref data, size_t size) {
        ASSERT_NONNULL(a != NULL);
        ASSERT_NONNULL(data != NULL);
        return memcpy(aoc_arena_alloc(a, size), data, size);
}

#ifndef AOCLIBS_ARENA_NOSTDIO
AOCLIBS_PREFIX char *aoc_arena_vsprintf(Arena *ref a, const char *format, va_list args) {
        ASSERT_NONNULL(a != NULL);
        va_list args_copy;
        va_copy(args_copy, args);
        int n = vsnprintf(NULL, 0, format, args_copy);
        va_end(args_copy);

        assert(n >= 0);
        char *result = (char *)aoc_arena_alloc(a, n + 1);
        vsnprintf(result, n + 1, format, args);

        return result;
}

AOCLIBS_PREFIX char *aoc_arena_sprintf(Arena *ref a, const char *format, ...) {
        ASSERT_NONNULL(a != NULL);
        va_list args;
        va_start(args, format);
        char *result = aoc_arena_vsprintf(a, format, args);
        va_end(args);

        return result;
}
#endif // AOCLIBS_ARENA_NOSTDIO

AOCLIBS_PREFIX void aoc_arena_reset(Arena *ref a) {
        ASSERT_NONNULL(a != NULL);
        for (Region *r = a->begin; r != NULL; r = r->next) {
                r->len = 0;
        }

        a->end = a->begin;
}

AOCLIBS_PREFIX void aoc_arena_destroy(Arena *ref a) {
        ASSERT_NONNULL(a != NULL);
        Region *r = a->begin;
        while (r) {
                Region *r0 = r;
                r = r->next;
                aoc_arena_free_region(r0);
        }
        a->begin = NULL;
        a->end = NULL;
}

AOCLIBS_PREFIX void aoc_arena_trim(Arena *ref a) {
        ASSERT_NONNULL(a != NULL);
        Region *r = a->end->next;
        while (r) {
                Region *r0 = r;
                r = r->next;
                aoc_arena_free_region(r0);
        }
        a->end->next = NULL;
}

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

#define arena_da_append aoc_arena_da_append
#define arena_da_append_cstr aoc_arena_da_append_cstr
#define arena_da_append_cstrl aoc_arena_da_append_cstrl
#define arena_da_append_many aoc_arena_da_append_many
#define arena_da_append_null aoc_arena_da_append_null
#endif

#endif // AOCLIBS_ARENA_H_
