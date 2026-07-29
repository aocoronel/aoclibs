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

typedef struct Allocator Allocator;
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

#ifdef AOC_IMPLEMENTATION

#ifndef NDEBUG
Heap_Trace_Entry *heap_trace_entry_head = NULL;
int heap_trace_alloc_count = 0;
int heap_trace_free_count = 0;
int heap_trace_failure_count = 0;
Source_Code_Location heap_trace_failure[HEAP_TRACE_MAX_FAILURE];
#endif

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

#ifdef THREAD
#define $thread_alloc_init()  \
	static void *_result;     \
                              \
	if (!is_thrd0()) {        \
		ckp;                  \
		void *_ptr = _result; \
		ckp;                  \
		return _ptr;          \
	}
#define $thread_alloc_return_error() \
	_result = NULL;                  \
	ckp;                             \
	ckp;                             \
	return NULL;
#define $thread_alloc_return(ptr) \
	_result = (ptr);              \
	ckp;                          \
	ckp;                          \
	return (ptr);
#else
#define $thread_alloc_init()
#define $thread_alloc_return_error() return NULL
#define $thread_alloc_return(ptr) return ptr
#endif

void *general_alloc(size_t size, $allocator $source_code_location) {
	$thread_alloc_init();
	void *ptr =
	    allocator->general.allocate(allocator->general.context, size $thread(*thread_count()));
	if (!ptr) {
		$heap_trace_failure();
		$thread_alloc_return_error();
	}
	$heap_trace_add_entry(ptr, size, source_code_location);
	$thread_alloc_return(ptr);
}

void *general_resize(void *ptr, size_t size, $allocator $source_code_location) {
	$assert_nonnull(ptr);

	$thread_alloc_init();

	void *new_ptr = allocator->general.reallocate(
	    allocator->general.context, ptr, size $thread(*thread_count()));
	if (!new_ptr) {
		$heap_trace_failure();
		$thread_alloc_return_error();
	}

	$heap_trace_remove_entry(ptr);

	$heap_trace_add_entry(new_ptr, size, source_code_location);
	$thread_alloc_return(new_ptr);
}

void general_dealloc(void *ptr, $allocator) {
#ifdef THREAD
	if (!is_thrd0()) return;
#endif
	$assert_nonnull(ptr);
	$heap_trace_remove_entry(ptr);
	allocator->general.deallocate(allocator->general.context, ptr);
}

void *buffer_alloc(size_t size, $allocator $source_code_location) {
	$thread_alloc_init();
	void *ptr =
	    allocator->buffer.allocate(allocator->buffer.context, size $thread(*thread_count()));
	if (!ptr) {
		$heap_trace_failure();
		$thread_alloc_return_error();
	}
	$heap_trace_add_entry(ptr, size, source_code_location);
	$thread_alloc_return(ptr);
}

void *buffer_resize(void *ptr, size_t oldsz, size_t newsz, $allocator $source_code_location) {
	$assert_nonnull(ptr);

	$thread_alloc_init();

	void *new_ptr = allocator->buffer.reallocate(
	    allocator->buffer.context, ptr, oldsz $thread(*thread_count()),
	    newsz $thread(*thread_count()));
	if (!new_ptr) {
		$heap_trace_failure();
		$thread_alloc_return_error();
	}

	$heap_trace_remove_entry(ptr);

	$heap_trace_add_entry(new_ptr, newsz, source_code_location);
	$thread_alloc_return(new_ptr);
}

void buffer_dealloc(void *ptr, $allocator) {
#ifdef THREAD
	if (!is_thrd0()) return;
#endif
	$assert_nonnull(ptr);
	$heap_trace_remove_entry(ptr);
	allocator->buffer.deallocate(allocator->buffer.context, ptr);
}

void buffer_reset($allocator) {
#ifdef THREAD
	if (!is_thrd0()) return;
#endif
	allocator->buffer.reset(allocator->buffer.context);
}

void buffer_destroy($allocator) {
#ifdef THREAD
	if (!is_thrd0()) return;
#endif
	allocator->buffer.destroy(allocator->buffer.context);
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

int heap_count_leaks(void) {
#ifdef NDEBUG
	return 0;
#else
	return heap_trace_alloc_count - heap_trace_free_count;
#endif
}

void heap_trace_summary(FILE *fd) {
#ifdef NDEBUG
	fprintf(fd, "Memory Report: Debugging is disabled\n");
#else
	int leaks, allocations, frees, failure;
	leaks = heap_count_leaks();
	allocations = heap_trace_alloc_count;
	frees = heap_trace_free_count;
	failure = heap_trace_failure_count;
	fprintf(
	    fd, "Memory Report: %d allocations, %d frees, %d leaks, %d failures\n", allocations, frees,
	    leaks, failure);
	if (leaks > 0) {
		fprintf(stderr, ">> Leaks:\n");
		Heap_Trace_Entry *curr = heap_trace_entry_head;
		while (curr) {
			fprintf(
			    fd, "%s:%d:%s: leaked %zu bytes (ptr: %p)\n", curr->loc.filename, curr->loc.line,
			    curr->loc.funcname, curr->size, curr->ptr);
			curr = curr->next;
		}
	}
#ifdef HEAP_TRACE_FAILURE_REPORT
	if (failure > 0) {
		fprintf(stderr, ">> Failures:\n");
		Source_Code_Location *curr = heap_trace_failure;
		$range(0, failure, i) {
			if (i >= HEAP_TRACE_MAX_FAILURE) break;
			fprintf(
			    fd, "%s:%d:%s: error messages WIP\n", curr->filename, curr->line, curr->funcname);
			curr++;
		}
	}
#endif //  HEAP_TRACE_FAILURE_REPORT
#endif // NDEBUG
}

#endif // AOC_IMPLEMENTATION

#if 0
void foo($allocator) {
	void *ptr = alloc(200);
	ptr = resize(ptr, 300);
	dealloc(ptr);
}

int main(int argc, char *argv[]) {
	// calls mmap
#ifdef ALLOC_VMEM
	{
		Virtual_Memory vm = {
			.addr = NULL,
			.flags = MAP_PRIVATE | MAP_ANONYMOUS,
			.prot = PROT_READ | PROT_WRITE,
			.fd = -1,
			.offset = 0,
			.remap_flags = MREMAP_MAYMOVE,
		};

		$define_allocator(LIBC_MMAP, &vm);
		void *ptr = alloc(200);
		ptr = resize(ptr, 300);
		dealloc(ptr);
	}
#endif

	// calls fixed buffer allocator
	{
		Fixed_Buffer fa = { .data = TEMPORARY_ALLOCATOR_GLOBAL_BUFFER,
			                .cap = TEMPORARY_ALLOCATOR_GLOBAL_BUFFER_CAPACITY };

		$define_allocator(FIXED_BUFFER_ALLOCATOR, &fa);
		void *ptr = balloc(200);
		ptr = bresize(ptr, 200, 300);
		bdealloc(ptr);
	}

	// calls malloc
	{
		$define_allocator(LIBC_MALLOC);
		foo(allocator);
	}

	heap_trace_summary(stderr);

	return 0;
}
#endif

#endif // AOC_ALLOC_H_
