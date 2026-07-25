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
    const General_Allocator_Alloc_Fn allocate;
    const General_Allocator_Realloc_Fn reallocate;
    const General_Allocator_Free_Fn deallocate;
    void *context;
};

// For buddy/arena/stack/bump allocators
struct Buffer_Allocator {
    const Buffer_Allocator_Alloc_Fn allocate;
    const Buffer_Allocator_Realloc_Fn reallocate;
    const Buffer_Allocator_Free_Fn deallocate;
    const Buffer_Allocator_Destroy_Fn destroy;
    const Buffer_Allocator_Reset_Fn reset;
    void *context;
};

struct Fixed_Buffer {
    size_t cap;
    size_t len;
    char *data;
};

#define LIBC_MALLOC libc_malloc, libc_realloc, libc_free
#define FIXED_BUFFER_ALLOCATOR fballoc, fbrealloc, fbdealloc, fbdestroy, fbreset

AOCDEF void *libc_malloc(void *ctx, size_t size);
AOCDEF void *libc_realloc(void *ctx, void *ptr, size_t size);
AOCDEF void libc_free(void *ctx, void *ptr);

#define TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY (8 * 1024 * 1024)

extern char TEMPORARY_ALLOCATOR_GLOBAL_BUFFER[TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY];

// Fixed-size buffer allocation
// Not thread-safe. This will only become thread-safe once I started to mess with threads in C,
// something I only played in Rust so far
AOCDEF void *fballoc(void *ctx, size_t size);
AOCDEF void *fbrealloc(void *ctx, void *ptr, size_t oldsz, size_t newsz);
// No-op. To reset just set Fixed_Buffer.len to 0
AOCDEF void fbdealloc(void *ctx, void *ptr);
AOCDEF void fbdestroy(void *ctx);
AOCDEF void fbreset(void *ctx);

#define $general_allocator const General_Allocator *general_allocator
#define $buffer_allocator const Buffer_Allocator *buffer_allocator

// These macros are an exception on the $macro() convention. Reason: too verbose
#define alloc(size) general_alloc((size), general_allocator $$source_code_location)
#define resize(ptr, size) general_resize((ptr), (size), general_allocator $$source_code_location)
#define dealloc(ptr) general_dealloc((ptr), general_allocator)
#define balloc(size) buffer_alloc((size), buffer_allocator $$source_code_location)
#define bresize(ptr, oldsz, newsz) \
    buffer_resize((ptr), (oldsz), (newsz), buffer_allocator $$source_code_location)
#define bdealloc(ptr) buffer_dealloc((ptr), buffer_allocator)
#define bdestroy() buffer_destroy(buffer_allocator)

AOCDEF void *general_alloc(size_t size, $general_allocator $source_code_location);
AOCDEF void *general_resize(void *ptr, size_t size, $general_allocator $source_code_location);
AOCDEF void general_dealloc(void *ptr, $general_allocator);

AOCDEF void *buffer_alloc(size_t size, $buffer_allocator $source_code_location);
AOCDEF void *
buffer_resize(void *ptr, size_t oldsz, size_t newsz, $buffer_allocator $source_code_location);
AOCDEF void buffer_dealloc(void *ptr, $buffer_allocator);
#define breset() buffer_reset(buffer_allocator)
AOCDEF void buffer_reset($buffer_allocator);

// alloc.h doesn't touch heap_entry in this function, so if there were initialization, the user
// must remove it from the entry manually
AOCDEF void buffer_destroy($buffer_allocator);

#define $define_general_allocator(...)                                        \
    const General_Allocator *general_allocator = &(const General_Allocator) { \
        __VA_ARGS__                                                           \
    }

#define $define_buffer_allocator(...)                                      \
    const Buffer_Allocator *buffer_allocator = &(const Buffer_Allocator) { \
        __VA_ARGS__                                                        \
    }

#ifdef ALLOC_VMEM
#ifndef _GNU_SOURCE
#error "ALLOC_VMEM requires _GNU_SOURCE"
#endif
typedef struct Virtual_Memory Virtual_Memory;

#include <errno.h>
#include <sys/mman.h>

struct Virtual_Memory {
    void *addr;
    int prot;
    int flags;
    int fd;
    off_t offset;

    int remap_flags;

    // libc_mmap and libc_mremap writes to this, so libc_munmap knows how to free the pages
    size_t size;
};

#define LIBC_MMAP libc_mmap, libc_mremap, libc_munmap

AOCDEF void *null libc_mmap(void *ctx, size_t size);
AOCDEF void *null libc_mremap(void *ctx, void *ptr, size_t size);
AOCDEF void libc_munmap(void *ctx, void *ptr);

#endif // ALLOC_VMEM

// Returns how many leaks found
#define heap_count_leaks heap_trace_alloc_count - heap_trace_free_count

struct Heap_Trace_Entry {
    void *ptr;
    size_t size;
    Source_Code_Location loc;
    struct Heap_Trace_Entry *next;
};

extern Heap_Trace_Entry *heap_trace_entry_head;
extern int heap_trace_alloc_count;
extern int heap_trace_free_count;

// Prints allocation and free count.
// If there was a leak, print the source of the leak
void heap_trace_summary(FILE *fd);

#ifdef NDEBUG
#define $heap_trace_add_entry(ptr, size, loc)
#define $heap_trace_remove_entry(ptr)
#else
#define $heap_trace_add_entry(ptr, size, loc) heap_trace_add_entry((ptr), (size), (loc))
#define $heap_trace_remove_entry(ptr) heap_trace_remove_entry((ptr))
#endif

void heap_trace_add_entry(void *ptr, size_t size $source_code_location);
void heap_trace_remove_entry(void *ptr);

#ifdef AOC_IMPLEMENTATION

Heap_Trace_Entry *heap_trace_entry_head = NULL;
int heap_trace_alloc_count = 0;
int heap_trace_free_count = 0;

char TEMPORARY_ALLOCATOR_GLOBAL_BUFFER[TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY];

#ifdef ALLOC_VMEM
void *libc_mmap(void *ctx, size_t size) {
    Virtual_Memory *vm = (Virtual_Memory *)ctx;
    Virtual_Memory v = *vm;

    vm->size = size;
    void *p = mmap(v.addr, size, v.prot, v.flags, v.fd, v.offset);
    if (p == MAP_FAILED) return NULL;
    return p;
}

void *libc_mremap(void *ctx, void *ptr, size_t size) {
    Virtual_Memory *vm = (Virtual_Memory *)ctx;
    size_t old_size = vm->size;
    vm->size = size;
    void *p = mremap(ptr, old_size, size, vm->remap_flags);
    if (p == MAP_FAILED) return NULL;
    return p;
}

void libc_munmap(void *ctx, void *ptr) {
    Virtual_Memory *vm = (Virtual_Memory *)ctx;
    // Deallocation should always succeed. If this fails, it's bad API usage
    $assert(munmap(ptr, vm->size) == 0, "%s", strerror(errno));
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

void *fballoc(void *ctx, size_t size) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;

    if (size + fa->len > fa->cap) return NULL;

    // We can either synchronize just here, or split the buffer across threads, so they never
    // touch each thread dedicated space
    size_t offset = fa->len;
    fa->len += size;

    char *ret = fa->data + offset;

    return ret;
}

void *fbrealloc(void *ctx, void *ptr, size_t oldsz, size_t newsz) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;

    if (newsz <= oldsz) return ptr;
    if (newsz + fa->len > fa->cap) return NULL;

    size_t offset = fa->len;
    fa->len += newsz;

    char *ret = fa->data + offset;

    memcpy(ret, ptr, oldsz);

    return ret;
}

void fbdealloc(void *ctx, void *ptr) {
    (void)ctx;
    (void)ptr;
}

void fbreset(void *ctx) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;
    fa->len = 0;
}

void fbdestroy(void *ctx) {
    Fixed_Buffer *fa = (Fixed_Buffer *)ctx;
    fa->len = 0;
}

void *general_alloc(size_t size, $general_allocator $source_code_location) {
    void *ptr = general_allocator->allocate(general_allocator->context, size);
    if (!ptr) return NULL;
    $heap_trace_add_entry(ptr, size, source_code_location);
    return ptr;
}

void *general_resize(void *ptr, size_t size, $general_allocator $source_code_location) {
    $assert_nonnull(ptr);

    void *new_ptr = general_allocator->reallocate(general_allocator->context, ptr, size);
    if (!new_ptr) return NULL;

    Heap_Trace_Entry **entry_ptr = &heap_trace_entry_head;
    $heap_trace_remove_entry(ptr);

    $heap_trace_add_entry(new_ptr, size, source_code_location);
    return new_ptr;
}

void general_dealloc(void *ptr, $general_allocator) {
    $assert_nonnull(ptr);
    $heap_trace_remove_entry(ptr);
    general_allocator->deallocate(general_allocator->context, ptr);
}

void *buffer_alloc(size_t size, $buffer_allocator $source_code_location) {
    void *ptr = buffer_allocator->allocate(buffer_allocator->context, size);
    if (!ptr) return NULL;
    $heap_trace_add_entry(ptr, size, source_code_location);
    return ptr;
}

void *
buffer_resize(void *ptr, size_t oldsz, size_t newsz, $buffer_allocator $source_code_location) {
    $assert_nonnull(ptr);

    void *new_ptr = buffer_allocator->reallocate(buffer_allocator->context, ptr, oldsz, newsz);
    if (!new_ptr) return NULL;

    Heap_Trace_Entry **entry_ptr = &heap_trace_entry_head;
    $heap_trace_remove_entry(ptr);

    $heap_trace_add_entry(new_ptr, newsz, source_code_location);
    return new_ptr;
}

void buffer_dealloc(void *ptr, $buffer_allocator) {
    $assert_nonnull(ptr);
    $heap_trace_remove_entry(ptr);
    buffer_allocator->deallocate(buffer_allocator->context, ptr);
}

void buffer_reset($buffer_allocator) {
    buffer_allocator->reset(buffer_allocator->context);
}

void buffer_destroy($buffer_allocator) {
    buffer_allocator->destroy(buffer_allocator->context);
}

#ifdef NDEBUG
void heap_trace_add_entry(void *ptr, size_t size $source_code_location) {
    return;
}

void heap_trace_remove_entry(void *ptr) {
    return;
}
#else
void heap_trace_add_entry(void *ptr, size_t size $source_code_location) {
    Heap_Trace_Entry *entry = (Heap_Trace_Entry *)malloc(sizeof(Heap_Trace_Entry));
    $assert(entry, "out of memory");

    entry->ptr = ptr;
    entry->size = size;
    entry->loc = source_code_location;
    entry->next = heap_trace_entry_head;
    heap_trace_entry_head = entry;
    heap_trace_alloc_count++;
}

void heap_trace_remove_entry(void *ptr) {
    Heap_Trace_Entry **curr = &heap_trace_entry_head;
    while (*curr) {
        if ((*curr)->ptr == ptr) {
            Heap_Trace_Entry *to_free = *curr;
            *curr = to_free->next;
            free(to_free);
            heap_trace_free_count++;
            return;
        }
        curr = &(*curr)->next;
    }
}
#endif

void heap_trace_summary(FILE *fd) {
#ifdef NDEBUG
    fprintf(fd, "Memory Report: Debugging is disabled\n");
#else
    int leaks, allocations, frees;
    leaks = heap_count_leaks;
    allocations = heap_trace_alloc_count;
    frees = heap_trace_free_count;
    fprintf(fd, "Memory Report: %d leaks, %d allocations, %d frees\n", leaks, allocations, frees);
    if (leaks == 0) return;
    Heap_Trace_Entry *curr = heap_trace_entry_head;
    while (curr) {
        fprintf(
            fd, "%zu bytes at %s() in %s:%d (ptr: %p)\n", curr->size, curr->loc.funcname,
            curr->loc.filename, curr->loc.line, curr->ptr);
        curr = curr->next;
    }
#endif
}

#endif // AOC_IMPLEMENTATION

// void foo($general_allocator) {
//     void *ptr = alloc(200);
//     ptr = resize(ptr, 300);
//     dealloc(ptr);
// }
//
// int main(int argc, char *argv[]) {
//     // calls mmap
// #ifdef ALLOC_VMEM
//     {
//         Virtual_Memory vm = {
//             .addr = NULL,
//             .flags = MAP_PRIVATE | MAP_ANONYMOUS,
//             .prot = PROT_READ | PROT_WRITE,
//             .fd = -1,
//             .offset = 0,
//             .remap_flags = MREMAP_MAYMOVE,
//         };
//
//         $define_general_allocator(LIBC_MMAP, &vm);
//         void *ptr = alloc(200);
//         ptr = resize(ptr, 300);
//         dealloc(ptr);
//     }
// #endif
//
//     // calls fixed buffer allocator
//     {
//         Fixed_Buffer fa = { .data = TEMPORARY_ALLOCATOR_GLOBAL_BUFFER,
//                             .cap = TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY };
//
//         $define_buffer_allocator(FIXED_BUFFER_ALLOCATOR, &fa);
//         void *ptr = balloc(200);
//         ptr = bresize(ptr, 200, 300);
//         bdealloc(ptr);
//     }
//
//     // calls malloc
//     {
//         $define_general_allocator(LIBC_MALLOC);
//         foo(general_allocator);
//     }
//
//     heap_trace_summary(stderr);
//
//     return 0;
// }

#endif // AOC_ALLOC_H_
