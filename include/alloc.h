#ifndef AOC_ALLOC_H_
#define AOC_ALLOC_H_

#include "base.h"

typedef void *(*General_Allocator_Alloc_Fn)(void *ctx, size_t size);
typedef void *(*General_Allocator_Realloc_Fn)(void *ctx, void *ptr, size_t size);
typedef void (*General_Allocator_Free_Fn)(void *ctx, void *ptr);

typedef void *(*Buffer_Allocator_Alloc_Fn)(void *ctx, size_t size);
typedef void *(*Buffer_Allocator_Realloc_Fn)(void *ctx, void *oldptr, size_t oldsz, size_t newsz);
typedef void (*Buffer_Allocator_Free_Fn)(void *ctx, void *ptr);
typedef void (*Buffer_Allocator_Destroy_Fn)(void *ctx);
typedef void (*Buffer_Allocator_Reset_Fn)(void *ctx);

typedef struct General_Allocator const General_Allocator;
typedef struct Buffer_Allocator const Buffer_Allocator;
typedef struct Context_Allocator const Context_Allocator;
typedef struct Fixed_Buffer Fixed_Buffer;

// General_Allocator vs Buffer_Allocator
//
// Something common in modern languages is a single allocator interface for every possible allocator
// implementation. This is a big mistake, because some APIs may need to allocate memory in a way
// that is very costly do to in some allocators.
//
// If you peak into map.h implementation you will notice that it currently uses Arena allocator,
// because if I was using just malloc, I would need to perform several syscalls to allocate AND to
// free, but the Arena simplifies this to two syscalls, which means that certain allocators could
// have no leaks, and others would cause leaks, and this is unreliable.

// For malloc/mmap
struct General_Allocator {
    const General_Allocator_Alloc_Fn alloc;
    const General_Allocator_Realloc_Fn realloc;
    const General_Allocator_Free_Fn free;
};

// For buddy/arena/stack/bump allocators
struct Buffer_Allocator {
    const Buffer_Allocator_Alloc_Fn alloc;
    const Buffer_Allocator_Realloc_Fn realloc;
    const Buffer_Allocator_Free_Fn free;
    const Buffer_Allocator_Destroy_Fn destroy;
    const Buffer_Allocator_Reset_Fn reset;
};

struct Context_Allocator {
    const General_Allocator allocator;
    void *context;
};

struct Fixed_Buffer {
    size_t cap;
    size_t len;
    char *data;
};

extern const General_Allocator LIBC_MALLOC;

AOCDEF void *libc_malloc(void *ctx, size_t size);
AOCDEF void *libc_realloc(void *ctx, void *ptr, size_t size);
AOCDEF void libc_free(void *ctx, void *ptr);

#define TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY (8 * 1024 * 1024)

extern char TEMPORARY_ALLOCATOR_GLOBAL_BUFFER[TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY];
extern const Buffer_Allocator FIXED_BUFFER_ALLOCATOR;

// Fixed-size buffer allocation
// Not thread-safe. This will only become thread-safe once I started to mess with threads in C,
// something I only played in Rust so far
AOCDEF void *balloc(void *ctx, size_t size);
AOCDEF void *brealloc(void *ctx, void *ptr, size_t oldsz, size_t newsz);
// No-op. To reset just set Fixed_Buffer.len to 0
AOCDEF void bfree(void *ctx, void *ptr);
AOCDEF void breset(void *ctx);
AOCDEF void bdestroy(void *ctx);

#define $alloc(size) alloc((size), context_allocator)
#define $resize(ptr, size) resize((ptr), (size), context_allocator)
#define $dealloc(ptr) dealloc((ptr), context_allocator)

#define $context_allocator const Context_Allocator *context_allocator

AOCDEF void *alloc(size_t size, $context_allocator);
AOCDEF void dealloc(void *ptr, $context_allocator);
AOCDEF void *resize(void *ptr, size_t size, $context_allocator);

#define $define_context_allocator(...)                                        \
    const Context_Allocator *context_allocator = &(const Context_Allocator) { \
        __VA_ARGS__                                                           \
    }

#ifdef ALLOC_VMEM
#ifndef _GNU_SOURCE
#error "ALLOC_VMEM requires _GNU_SOURCE"
#endif
typedef struct Virtual_Memory Virtual_Memory;

#include <errno.h>
#include <sys/mman.h>

struct Virtual_Memory {
    void *__addr;
    int __prot;
    int __flags;
    int __fd;
    off_t __offset;

    int __remap_flags;

    // libc_mmap and libc_mremap writes to this, so libc_munmap knows how to free the pages
    size_t __size;
};

extern const General_Allocator LIBC_MMAP;

AOCDEF void *null libc_mmap(void *ctx, size_t size);
AOCDEF void *null libc_mremap(void *ctx, void *ptr, size_t size);
AOCDEF void libc_munmap(void *ctx, void *ptr);

#endif // ALLOC_VMEM

#ifdef AOC_IMPLEMENTATION

char TEMPORARY_ALLOCATOR_GLOBAL_BUFFER[TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY];

const General_Allocator LIBC_MALLOC = { libc_malloc, libc_realloc, libc_free };
const Buffer_Allocator FIXED_BUFFER_ALLOCATOR = { balloc, brealloc, bfree, bdestroy, breset };

#ifdef ALLOC_VMEM

const General_Allocator LIBC_MMAP = { libc_mmap, libc_mremap, libc_munmap };

void *libc_mmap(void *ctx, size_t size) {
    struct Virtual_Memory *vm = (struct Virtual_Memory *)ctx;
    struct Virtual_Memory v = *vm;

    vm->__size = size;
    void *p = mmap(v.__addr, size, v.__prot, v.__flags, v.__fd, v.__offset);
    if (p == MAP_FAILED) return NULL;
    return p;
}

void *libc_mremap(void *ctx, void *ptr, size_t size) {
    struct Virtual_Memory *vm = (struct Virtual_Memory *)ctx;
    struct Virtual_Memory v = *vm;

    size_t old_size = vm->__size;
    vm->__size = size;
    void *p = mremap(ptr, old_size, size, vm->__remap_flags);
    if (p == MAP_FAILED) return NULL;
    return p;
}

void libc_munmap(void *ctx, void *ptr) {
    struct Virtual_Memory *vm = (struct Virtual_Memory *)ctx;
    struct Virtual_Memory v = *vm;

    int ret = munmap(ptr, v.__size);
    if (ret != 0) {
        fprintf(stderr, "%s\n", strerror(errno));
        abort();
    }
}
#endif // ALLOC_VMEM

void *libc_malloc(void *ctx, size_t size) {
    (void)ctx;
    return malloc(size);
}

void *libc_realloc(void *ctx, void *ptr, size_t size) {
    (void)ctx;
    return realloc(ptr, size);
}

void libc_free(void *ctx, void *ptr) {
    (void)ctx;
    free(ptr);
}

void *balloc(void *ctx, size_t size) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;

    if (size + fa->len > fa->cap) return NULL;

    // We can either synchronize just here, or split the buffer across threads, so they never
    // touch each thread dedicated space
    size_t offset = fa->len;
    fa->len += size;

    char *ret = fa->data + offset;

    return ret;
}

void *brealloc(void *ctx, void *ptr, size_t oldsz, size_t newsz) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;

    if (newsz <= oldsz) return ptr;
    if (newsz + fa->len > fa->cap) return NULL;

    size_t offset = fa->len;
    fa->len += newsz;

    char *ret = fa->data + offset;

    memcpy(ret, ptr, oldsz);

    return ret;
}

void bfree(void *ctx, void *ptr) {
    (void)ctx;
    (void)ptr;
}

void breset(void *ctx) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;
    fa->len = 0;
}

void bdestroy(void *ctx) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;
    fa->len = 0;
}

void *alloc(size_t size, $context_allocator) {
    return context_allocator->allocator.alloc(context_allocator->context, size);
}

void *resize(void *ptr, size_t size, $context_allocator) {
    return context_allocator->allocator.realloc(context_allocator->context, ptr, size);
}

void dealloc(void *ptr, $context_allocator) {
    context_allocator->allocator.free(context_allocator->context, ptr);
}

#endif // AOC_IMPLEMENTATION

// void foo($context_allocator) {
//     void *ptr = $alloc(200);
//     ptr = $resize(ptr, 300);
//     $dealloc(ptr);
// }
//
// int main(int argc, char *argv[]) {
//     // calls mmap
// #ifdef ALLOC_VMEM
//     {
//         Virtual_Memory vm = {
//             .__addr = NULL,
//             .__flags = MAP_PRIVATE | MAP_ANONYMOUS,
//             .__prot = PROT_READ | PROT_WRITE,
//             .__fd = -1,
//             .__offset = 0,
//             .__remap_flags = MREMAP_MAYMOVE,
//         };
//         $define_context_allocator(LIBC_MMAP, &vm);
//         void *ptr = $alloc(200);
//         ptr = $resize(ptr, 300);
//         $dealloc(ptr);
//     }
// #endif
//
//     // calls fixed buffer allocator
//     {
//         Fixed_Buffer fa = { .buffer = TEMPORARY_ALLOCATOR_GLOBAL_BUFFER,
//                             .cap = TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY };
//         Context_Allocator *a = &(Context_Allocator){ FIXED_BUFFER_ALLOCATOR, &fa };
//         void *ptr = alloc(200, a);
//         ptr = resize(ptr, 300, a);
//         dealloc(ptr, a);
//     }
//
//     // calls malloc
//     {
//         $define_context_allocator(LIBC_MALLOC);
//         foo(context_allocator);
//     }
//
//     return 0;
// }

#endif // AOC_ALLOC_H_
