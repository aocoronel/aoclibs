#ifndef AOC_ALLOC_H_
#define AOC_ALLOC_H_

#include "base.h"
#include "thread.h"
#include <errno.h>

typedef void *(*General_Allocator_Alloc_Fn)(void *ctx, size_t size);
typedef void *(*General_Allocator_Realloc_Fn)(void *ctx, void *ptr, size_t size);
typedef void (*General_Allocator_Free_Fn)(void *ctx, void *ptr);

typedef void *(*Buffer_Allocator_Alloc_Fn)(void *ctx, size_t size);
typedef void *(*Buffer_Allocator_Realloc_Fn)(void *ctx, void *oldptr, size_t oldsz, size_t newsz);
typedef void (*Buffer_Allocator_Free_Fn)(void *ctx, void *ptr);
typedef void (*Buffer_Allocator_Destroy_Fn)(void *ctx);
typedef void (*Buffer_Allocator_Reset_Fn)(void *ctx);

typedef enum Allocator_Mode {
	General_Allocator_Mode,
	Buffer_Allocator_Mode,
} Allocator_Mode;

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

// I have a slab allocator implementation sitting in my todo for a while, but I suppose it requires
// its own allocator interface, since the API doesn't follow the buffer allocator interface

struct Allocator {
	General_Allocator general;
	Buffer_Allocator buffer;
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

#define $allocator const Allocator *allocator

// These macros are an exception on the $macro() convention. Reason: too verbose
#define alloc(size) general_alloc((size), allocator $$source_code_location)
#define resize(ptr, size) general_resize((ptr), (size), allocator $$source_code_location)
#define dealloc(ptr) general_dealloc((ptr), allocator)
#define balloc(size) buffer_alloc((size), allocator $$source_code_location)
#define bresize(ptr, oldsz, newsz) \
	buffer_resize((ptr), (oldsz), (newsz), allocator $$source_code_location)
#define bdealloc(ptr) buffer_dealloc((ptr), allocator)
#define bdestroy() buffer_destroy(allocator)

AOCDEF void *general_alloc(size_t size, $allocator $source_code_location);
AOCDEF void *general_resize(void *ptr, size_t size, $allocator $source_code_location);
AOCDEF void general_dealloc(void *ptr, $allocator);

AOCDEF void *buffer_alloc(size_t size, $allocator $source_code_location);
AOCDEF void *buffer_resize(void *ptr, size_t oldsz, size_t newsz, $allocator $source_code_location);
AOCDEF void buffer_dealloc(void *ptr, $allocator);
#define breset() buffer_reset(allocator)
AOCDEF void buffer_reset($allocator);

// alloc.h doesn't touch heap_entry in this function, so if there were initialization, the user
// must remove it from the entry manually
AOCDEF void buffer_destroy($allocator);

#define $define_allocator(...)                        \
	const Allocator *allocator = &(const Allocator) { \
		__VA_ARGS__                                   \
	}

// TODO: mmap is not particularly efficient for small allocations, so making it a general_allocator
// may be a bad idea, perhaps buffer allocators could take advantage of it?
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

// For reference, so I don't forget how to use mmap
// Virtual_Memory vm = {
//     .addr = NULL,
//     .flags = MAP_PRIVATE | MAP_ANONYMOUS,
//     .prot = PROT_READ | PROT_WRITE,
//     .fd = -1,
//     .offset = 0,
//     .remap_flags = MREMAP_MAYMOVE,
// };
AOCDEF void *null libc_mmap(void *ctx, size_t size);
AOCDEF void *null libc_mremap(void *ctx, void *ptr, size_t size);
AOCDEF void libc_munmap(void *ctx, void *ptr);

#endif // ALLOC_VMEM

struct Heap_Trace_Entry {
	void *ptr;
	size_t size;
	Source_Code_Location loc;
	struct Heap_Trace_Entry *next;
};

#ifndef NDEBUG
extern Heap_Trace_Entry *heap_trace_entry_head;
extern int heap_trace_alloc_count;
extern int heap_trace_free_count;
extern int heap_trace_failure_count;
#endif

#define HEAP_TRACE_MAX_FAILURE 32
extern Source_Code_Location heap_trace_failure[HEAP_TRACE_MAX_FAILURE];

// Prints allocation and free count.
// If there was a leak, print the source of the leak
void heap_trace_summary(FILE *fd);

int heap_count_leaks(void);

#ifdef NDEBUG
#define $heap_trace_add_entry(ptr, size, loc)
#define $heap_trace_remove_entry(ptr)
#define $heap_trace_failure()
#else
#ifdef HEAP_TRACE_FAILURE_REPORT
#define $heap_trace_failure()                                                    \
	do {                                                                         \
		if (heap_trace_failure_count < HEAP_TRACE_MAX_FAILURE) {                 \
			heap_trace_failure[heap_trace_failure_count] = source_code_location; \
			heap_trace_failure_count++;                                          \
		}                                                                        \
	} while (0)
#else
#define $heap_trace_failure()
#endif // HEAP_TRACE_FAILURE_REPORT
#define $heap_trace_add_entry(ptr, size, loc) heap_trace_add_entry((ptr), (size), (loc))
#define $heap_trace_remove_entry(ptr) heap_trace_remove_entry((ptr))
#endif

void heap_trace_add_entry(void *ptr, size_t size $source_code_location);
void heap_trace_remove_entry(void *ptr);

#define $memdup(src, size, mode) memdup((src), (size), (mode), allocator)
// void *dupped = $memdup(cstring, strlen(cstring), General_Allocator_Mode);
AOCDEF void *null memdup(void *src, size_t size, Allocator_Mode mode, $allocator);

#ifdef AOC_IMPLEMENTATION
#include "alloc.c"
#endif // AOC_IMPLEMENTATION

#endif // AOC_ALLOC_H_
