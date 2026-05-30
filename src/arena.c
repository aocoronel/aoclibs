#pragma once

#include "arena.h"
#include "base.h"

#if AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_LIBC_MALLOC

AOCDEF Region *arena_new_region(const size_t capacity) {
    const size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;

    Region *r = (Region *)malloc(size_bytes);
    if (!r) return NULL;

    r->next = NULL;
    r->len = 0;
    r->cap = capacity;
    return r;
}

AOCDEF void arena_free_region(Region *r) {
    ASSERT_NONNULL(r);
    free(r);
}

#elif AOCLIBS_ARENA_BACKEND == AOCLIBS_ARENA_BACKEND_VIRTUAL_ALLOC && __linux__

#include <unistd.h>
#include <sys/mman.h>

AOCDEF Region *arena_new_region(const size_t capacity) {
    const size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * capacity;

    Region *r = mmap(NULL, size_bytes, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (!r) return NULL;

    r->next = NULL;
    r->len = 0;
    r->cap = capacity;
    return r;
}

AOCDEF void arena_free_region(Region *r) {
    ASSERT_NONNULL(r);
    const size_t size_bytes = sizeof(Region) + sizeof(uintptr_t) * r->cap;
    int ret = munmap(r, size_bytes);

    // Manpage:
    // "On success, munmap() returns 0.  On failure, it returns -1, and errno
    //  is set to indicate the error (probably to EINVAL)."
    //
    // For consistency, we assume the user always provide a valid address
    ASSERT(ret == 0, "%s", strerror(errno));
}

#else

#error "Supported arenas: AOCLIBS_ARENA_BACKEND_LIBC_MALLOC and AOCLIBS_ARENA_BACKEND_VIRTUAL_ALLOC"

#endif

AOCDEF void *arena_alloc(Arena *a, const size_t size_bytes) {
    ASSERT_NONNULL(a != NULL);
    const size_t size = (size_bytes + sizeof(uintptr_t) - 1) / sizeof(uintptr_t);

    if (a->end == NULL) {
        ASSERT(a->begin == NULL);
        size_t capacity = AOCLIBS_ARENA_REGION_DEFAULT_CAPACITY;
        if (capacity < size) capacity = size;

        a->end = arena_new_region(capacity);
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
        a->end->next = arena_new_region(capacity);
        if (a->end->next == NULL) return NULL;
        a->end = a->end->next;
    }

    void *result = &a->end->data[a->end->len];
    a->end->len += size;
    return result;
}

AOCDEF void *arena_calloc(Arena *a, const size_t size_bytes) {
    ASSERT_NONNULL(a);

    void *ptr = arena_alloc(a, size_bytes);
    if (!ptr) return NULL;

    memset(ptr, 0, size_bytes);
    return ptr;
}

AOCDEF void *arena_realloc(Arena *a, void *oldptr, const size_t oldsz, const size_t newsz) {
    ASSERT_NONNULL(a != NULL);
    if (newsz <= oldsz) return oldptr;

    void *newptr = arena_alloc(a, newsz);
    if (newptr == NULL) return NULL;

    char *newptr_char = (char *)newptr;
    char *oldptr_char = (char *)oldptr;
    memcpy(newptr_char, oldptr_char, oldsz);
    return newptr;
}

AOCDEF void *arena_memdup(Arena *a, void *data, const size_t size) {
    ASSERT_NONNULL(a != NULL);
    ASSERT_NONNULL(data != NULL);

    void *p = arena_alloc(a, size);
    if (!p) return NULL;

    memcpy(p, data, size);

    return p;
}

AOCDEF char *arena_vsprintf(Arena *a, const char *format, va_list args) {
    ASSERT_NONNULL(a != NULL);
    ASSERT_NONNULL(format);

    va_list args_copy;
    va_copy(args_copy, args);
    int n = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    ASSERT(n >= 0);

    char *result = (char *)arena_alloc(a, n + 1);
    if (!result) return NULL;

    vsnprintf(result, n + 1, format, args);

    return result;
}

AOCDEF char *arena_sprintf(Arena *a, const char *format, ...) {
    ASSERT_NONNULL(a != NULL);
    ASSERT_NONNULL(format);

    va_list args;
    va_start(args, format);
    char *result = arena_vsprintf(a, format, args);
    va_end(args);

    return result;
}

AOCDEF void arena_reset(Arena *a) {
    ASSERT_NONNULL(a != NULL);
    for (Region *r = a->begin; r != NULL; r = r->next) {
        r->len = 0;
    }

    a->end = a->begin;
}

AOCDEF void arena_destroy(Arena *a) {
    ASSERT_NONNULL(a != NULL);
    Region *r = a->begin;
    while (r) {
        Region *r0 = r;
        r = r->next;
        arena_free_region(r0);
    }
    a->begin = NULL;
    a->end = NULL;
}

AOCDEF void arena_trim(Arena *a) {
    ASSERT_NONNULL(a != NULL);
    Region *r = a->end->next;
    while (r) {
        Region *r0 = r;
        r = r->next;
        arena_free_region(r0);
    }
    a->end->next = NULL;
}
