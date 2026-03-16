#pragma once

#include "arena.h"
#include <stdio.h>

#include <stddef.h>
#include <string.h>

#if AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_LIBC_MALLOC

#include <stdlib.h>

AOCDEF Region *aoc_arena_new_region(size_t capacity) {
    size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;
    Region *r = (Region *)malloc(size_bytes);
    ASSERT(r, "Out of memory");
    r->next = NULL;
    r->len = 0;
    r->cap = capacity;
    return r;
}

AOCDEF void aoc_arena_free_region(Region *r) {
    ASSERT_NONNULL(r != NULL);
    free(r);
}

#elif AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_VIRTUAL_ALLOC && __linux__

#include <unistd.h>
#include <sys/mman.h>

AOCDEF Region *aoc_arena_new_region(size_t capacity) {
    size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;
    Region *r = mmap(NULL, size_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    ASSERT(r != MAP_FAILED, "Failed to allocate virtual memory");
    r->next = NULL;
    r->len = 0;
    r->cap = capacity;
    return r;
}

AOCDEF void aoc_arena_free_region(Region *r) {
    size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * r->cap;
    int ret = munmap(r, size_bytes);
    ASSERT(ret == 0);
}

#elif AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_VIRTUAL_ALLOC && _WIN32

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define INV_HANDLE(x) (((x) == NULL) || ((x) == INVALID_HANDLE_VALUE))

AOCDEF Region *aoc_arena_new_region(size_t capacity) {
    SIZE_T size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;
    Region *r = VirtualAllocEx(GetCurrentProcess(), /* Allocate in current process address space */
                               NULL, /* Unknown position */
                               size_bytes, /* Bytes to allocate */
                               MEM_COMMIT | MEM_RESERVE, /* Reserve and commit allocated page */
                               PAGE_READWRITE /* Permissions ( Read/Write )*/
    );
    if (INV_HANDLE(r)) ASSERT(0, "VirtualAllocEx() failed.");

    r->next = NULL;
    r->len = 0;
    r->cap = capacity;
    return r;
}

AOCDEF void aoc_arena_free_region(Region *r) {
    if (INV_HANDLE(r)) return;

    BOOL free_result =
            VirtualFreeEx(GetCurrentProcess(), /* Deallocate from current process address space */
                          (LPVOID)r, /* Address to deallocate */
                          0, /* Bytes to deallocate ( Unknown, deallocate entire page ) */
                          MEM_RELEASE /* Release the page ( And implicitly decommit it ) */
            );

    if (FALSE == free_result) ASSERT(0, "VirtualFreeEx() failed.");
}

#else

#error "Unknown Arena backend"

#endif

AOCDEF void *aoc_arena_alloc(Arena *a, size_t size_bytes) {
    ASSERT_NONNULL(a != NULL);
    size_t size = (size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

    if (a->end == NULL) {
        ASSERT(a->begin == NULL);
        size_t capacity = AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        a->end = aoc_arena_new_region(capacity);
        if (a->end == NULL) return NULL;
        a->begin = a->end;
    }

    while (a->end->len + size > a->end->cap && a->end->next != NULL) {
        a->end = a->end->next;
    }

    if (a->end->len + size > a->end->cap) {
        ASSERT(a->end->next == NULL);
        size_t capacity = AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;
        a->end->next = aoc_arena_new_region(capacity);
        if (a->end->next == NULL) return NULL;
        a->end = a->end->next;
    }

    void *result = &a->end->data[a->end->len];
    a->end->len += size;
    return result;
}

AOCDEF void *aoc_arena_calloc(Arena *a, size_t size_bytes) {
    void *ptr = aoc_arena_alloc(a, size_bytes);
    memset(ptr, 0, size_bytes);
    return ptr;
}

AOCDEF void *aoc_arena_realloc(Arena *a, void *oldptr, size_t oldsz, size_t newsz) {
    ASSERT_NONNULL(a != NULL);
    if (newsz <= oldsz) return oldptr;
    void *newptr = aoc_arena_alloc(a, newsz);
    if (newptr == NULL) return NULL;
    char *newptr_char = (char *)newptr;
    char *oldptr_char = (char *)oldptr;
    memcpy(newptr_char, oldptr_char, oldsz);
    return newptr;
}

AOCDEF void *aoc_arena_memdup(Arena *a, void *data, size_t size) {
    ASSERT_NONNULL(a != NULL);
    ASSERT_NONNULL(data != NULL);
    return memcpy(aoc_arena_alloc(a, size), data, size);
}

AOCDEF char *aoc_arena_vsprintf(Arena *a, const char *format, va_list args) {
    ASSERT_NONNULL(a != NULL);
    va_list args_copy;
    va_copy(args_copy, args);
    int n = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    ASSERT(n >= 0);
    char *result = (char *)aoc_arena_alloc(a, n + 1);
    vsnprintf(result, n + 1, format, args);

    return result;
}

AOCDEF char *aoc_arena_sprintf(Arena *a, const char *format, ...) {
    ASSERT_NONNULL(a != NULL);
    va_list args;
    va_start(args, format);
    char *result = aoc_arena_vsprintf(a, format, args);
    va_end(args);

    return result;
}

AOCDEF void aoc_arena_reset(Arena *a) {
    ASSERT_NONNULL(a != NULL);
    for (Region *r = a->begin; r != NULL; r = r->next) {
        r->len = 0;
    }

    a->end = a->begin;
}

AOCDEF void aoc_arena_destroy(Arena *a) {
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

AOCDEF void aoc_arena_trim(Arena *a) {
    ASSERT_NONNULL(a != NULL);
    Region *r = a->end->next;
    while (r) {
        Region *r0 = r;
        r = r->next;
        aoc_arena_free_region(r0);
    }
    a->end->next = NULL;
}
