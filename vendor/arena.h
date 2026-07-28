#ifndef AOC_ARENA_H_
#define AOC_ARENA_H_

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
#include "thread.h"

typedef struct Arena_Region Arena_Region;
typedef struct Arena Arena;

struct Arena_Region {
    Arena_Region *null next;
    size_t len;
    size_t cap;
    uintptr_t data[];
};

struct Arena {
    Arena_Region *null begin, *null end;
};

// Allocates a new region in the heap with given "capacity".
//
// This memory is freed using "arena_free_region".
//
// NULL :: failed to allocate
AOCDEF Arena_Region *null arena_new_region(const size_t capacity);
AOCDEF void arena_free_region(Arena_Region *r);

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
AOCDEF void *null arena_realloc(
    Arena *restrict a, void *null restrict oldptr, const size_t oldsz, const size_t newsz);

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

// Dynamic Arena

#define dar_reserve(a, da, new_cap) _dar_reserve(a, da, new_cap, sizeof(*(da)->data))

#define _dar_reserve(a, da, new_cap, sizeof_da)                                                  \
    do {                                                                                         \
        if ($unlikely((da)->len >= (da)->cap)) {                                                 \
            size_t new_capacity = (da)->cap < CONFIG_ARENA_DA_DEFAULT_CAPACITY ?                 \
                                      CONFIG_ARENA_DA_DEFAULT_CAPACITY :                         \
                                      new_cap;                                                   \
            while ((new_cap) > new_capacity) {                                                   \
                new_capacity *= 2;                                                               \
            }                                                                                    \
            (da)->data = (typeof((da)->data))arena_realloc(                                      \
                (a), (da)->data, (da)->cap * (sizeof_da), new_capacity * (sizeof_da));           \
            $assert((da)->data, "out of memory while reserving memory for arena dynamic array"); \
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

#define arc_lcat(a, rc, cstr) dar_append(a, rc, cstr, $strlen(cstr))
#define arc_cat(a, rc, cstr, len) dar_append(a, rc, cstr, len)

#define arc_lappend(a, rc, items_buff) arc_append(a, rc, items_buff, $strlen(items_buff))
#define arc_append(a, rc, items_buff, items_size)                                         \
    do {                                                                                  \
        dar_reserve(a, (rc), 1 + (rc)->len + (items_size));                               \
        (rc)->data[(rc)->len++] = ' ';                                                    \
        memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
        (rc)->len += (items_size);                                                        \
    } while (0)

#define arc_appendf(a, rc, fmt, ...)                                                         \
    do {                                                                                     \
        const int needed = cstr_fmt_size(fmt, __VA_ARGS__);                                  \
        dar_reserve(a, rc, (rc)->len + needed);                                              \
        const int written =                                                                  \
            cstr_fmt_write((rc)->data + (rc)->len, (rc)->cap - (rc)->len, fmt, __VA_ARGS__); \
        (rc)->len += written;                                                                \
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

#define arc_lcat_fast(a, rc, cstr) dar_append_fast(a, rc, cstr, $strlen(cstr))
#define arc_cat_fast(a, rc, cstr, len) dar_append_fast(a, rc, cstr, len)

#define arc_lappend_fast(a, rc, items_buff) arc_append(a, rc, items_buff, $strlen(items_buff))
#define arc_append_fast(a, rc, items_buff, items_size)                                    \
    do {                                                                                  \
        (rc)->data[(rc)->len++] = ' ';                                                    \
        memcpy((rc)->data + (rc)->len, (items_buff), (items_size) * sizeof(*(rc)->data)); \
        (rc)->len += (items_size);                                                        \
    } while (0)
// =================================

#ifdef AOC_IMPLEMENTATION

#if AOC_ARENA_BACKEND == AOC_ARENA_BACKEND_LIBC_MALLOC

AOCDEF Arena_Region *arena_new_region(const size_t capacity) {
    const size_t size_bytes = sizeof(Arena_Region) + sizeof(uintptr_t) * capacity;

    Arena_Region *r = (Arena_Region *)malloc(size_bytes);
    if (!r) return NULL;

    // Properly allocated memory will slowly releasing this
    $sanitizer_poison_memory(r->data, capacity);

    r->next = NULL;
    r->len = 0;
    r->cap = capacity;
    return r;
}

AOCDEF void arena_free_region(Arena_Region *r) {
    $assert_nonnull(r);
    free(r);
}

#elif AOC_ARENA_BACKEND == AOC_ARENA_BACKEND_VIRTUAL_ALLOC && __linux__

#include <unistd.h>
#include <sys/mman.h>

AOCDEF Arena_Region *arena_new_region(const size_t capacity) {
    const size_t size_bytes = sizeof(Arena_Region) + sizeof(uintptr_t) * capacity;

    Arena_Region *r =
        mmap(NULL, size_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (!r) return NULL;

    r->next = NULL;
    r->len = 0;
    r->cap = capacity;
    return r;
}

AOCDEF void arena_free_region(Arena_Region *r) {
    $assert_nonnull(r);
    const size_t size_bytes = sizeof(Arena_Region) + sizeof(uintptr_t) * r->cap;
    int ret = munmap(r, size_bytes);

    // Manpage:
    // "On success, munmap() returns 0.  On failure, it returns -1, and errno
    //  is set to indicate the error (probably to EINVAL)."
    //
    // For consistency, we assume the user always provide a valid address
    $assert(ret == 0, "%s", strerror(errno));
}

#else

#error \
    "Only malloc (AOC_ARENA_BACKEND_LIBC_MALLOC) and mmap (AOC_ARENA_BACKEND_VIRTUAL_ALLOC) are supported."

#endif

AOCDEF void *arena_alloc(Arena *a, const size_t size_bytes) {
    $assert_nonnull(a != NULL);

#ifdef THREAD
    static void *result;

    if (!is_thrd0()) {
        ckp;
        void *ptr = result;
        ckp;
        return ptr;
    }
#endif

    const size_t size = (size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t) $thread(* thread_count());

    if (a->end == NULL) {
        $assert(a->begin == NULL);
        size_t capacity = CONFIG_ARENA_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;

        a->end = arena_new_region(capacity);
        if (a->end == NULL) {
#ifdef THREAD
            result = NULL;
            ckp;
            ckp;
            return result;
#else
            return NULL;
#endif
        }

        a->begin = a->end;
    }

    while (a->end->len + size > a->end->cap && a->end->next != NULL) {
        a->end = a->end->next;
    }

    if (a->end->len + size > a->end->cap) {
        $assert(a->end->next == NULL);
        size_t capacity = CONFIG_ARENA_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        a->end->next = arena_new_region(capacity);
        if (a->end->next == NULL) {
#ifdef THREAD
            result = NULL;
            ckp;
            ckp;
            return result;
#else
            return NULL;
#endif
        }
        a->end = a->end->next;
    }

#ifdef HAVE_SANITIZER
    {
        size_t rounded =
            ((size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t)) * sizeof(uintptr_t);
        void *ptr = &a->end->data[a->end->len];
        $sanitizer_unpoison_memory(ptr, size_bytes);
        if (rounded > size_bytes)
            $sanitizer_poison_memory((char *)ptr + size_bytes, rounded - size_bytes);
    }
#endif

#ifdef THREAD
    result = &a->end->data[a->end->len];
    a->end->len += size;
    ckp;
    ckp;
    return result;
#else
    void *result = &a->end->data[a->end->len];
    a->end->len += size;
    return result;
#endif
}

AOCDEF void *arena_calloc(Arena *a, const size_t size_bytes) {
    $assert_nonnull(a);

    void *ptr = arena_alloc(a, size_bytes);
    if (!ptr) return NULL;

    memset(ptr, 0, size_bytes);
    return ptr;
}

AOCDEF void *arena_realloc(Arena *a, void *oldptr, const size_t oldsz, const size_t newsz) {
    $assert_nonnull(a != NULL);
    if (newsz <= oldsz) return oldptr;

    void *newptr = arena_alloc(a, newsz);
    if (newptr == NULL) return NULL;

    char *newptr_char = (char *)newptr;
    char *oldptr_char = (char *)oldptr;
    memcpy(newptr_char, oldptr_char, oldsz);
    return newptr;
}

AOCDEF void *arena_memdup(Arena *a, void *data, const size_t size) {
    $assert_nonnull(a != NULL);
    $assert_nonnull(data != NULL);

    void *p = arena_alloc(a, size);
    if (!p) return NULL;

    memcpy(p, data, size);

    return p;
}

AOCDEF char *arena_vsprintf(Arena *a, const char *format, va_list args) {
    $assert_nonnull(a != NULL);
    $assert_nonnull(format);

    va_list args_copy;
    va_copy(args_copy, args);
    int n = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    $assert(n >= 0);

    char *result = (char *)arena_alloc(a, n + 1);
    if (!result) return NULL;

    vsnprintf(result, n + 1, format, args);

    return result;
}

AOCDEF char *arena_sprintf(Arena *a, const char *format, ...) {
    $assert_nonnull(a != NULL);
    $assert_nonnull(format);

    va_list args;
    va_start(args, format);
    char *result = arena_vsprintf(a, format, args);
    va_end(args);

    return result;
}

AOCDEF void arena_reset(Arena *a) {
    $assert_nonnull(a != NULL);
    for (Arena_Region *r = a->begin; r != NULL; r = r->next) {
        r->len = 0;
    }

    a->end = a->begin;
}

AOCDEF void arena_destroy(Arena *a) {
    $assert_nonnull(a != NULL);
#ifdef THREAD
    if (!is_thrd0()) return;
#endif
    Arena_Region *r = a->begin;
    while (r) {
        Arena_Region *r0 = r;
        r = r->next;
        arena_free_region(r0);
    }
    a->begin = NULL;
    a->end = NULL;
}
#endif // AOC_IMPLEMENTATION

#endif // AOC_ARENA_H_
