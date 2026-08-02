#pragma once

#include "alloc.h"
#include "thread.h"

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

#ifdef THREAD
#define $thread_alloc_assert_only_thrd0(fn)                                                       \
	$assert(                                                                                      \
	    is_thrd0(), #fn "() must be called only by thrd0. "                                       \
	                    "This assertion is here to ensure you properly synchronize all threads, " \
	                    "and make sure they no longer need the memory")
#else
#define $thread_alloc_assert_only_thrd0(fn)
#endif

void *general_alloc(size_t size, $allocator $source_code_location) {
	$thread_alloc_init();
	const size_t allocation_size = size;
	void *ptr = allocator->general.allocate(allocator->general.context, allocation_size);
	if (!ptr) {
		$heap_trace_failure();
		$thread_alloc_return_error();
	}
	$heap_trace_add_entry(ptr, allocation_size, source_code_location);
	$thread_alloc_return(ptr);
}

void *general_resize(void *ptr, size_t size, $allocator $source_code_location) {
	$assert_nonnull(ptr);

	$thread_alloc_init();

	const size_t allocation_size = size;
	void *new_ptr = allocator->general.reallocate(allocator->general.context, ptr, allocation_size);
	if (!new_ptr) {
		$heap_trace_failure();
		$thread_alloc_return_error();
	}

	$heap_trace_remove_entry(ptr);

	$heap_trace_add_entry(new_ptr, allocation_size, source_code_location);
	$thread_alloc_return(new_ptr);
}

void general_dealloc(void *ptr, $allocator) {
	$thread_alloc_assert_only_thrd0(dealloc);
	$assert_nonnull(ptr);
	$heap_trace_remove_entry(ptr);
	allocator->general.deallocate(allocator->general.context, ptr);
}

void *buffer_alloc(size_t size, $allocator $source_code_location) {
	$thread_alloc_init();
	void *ptr = allocator->buffer.allocate(allocator->buffer.context, size);
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

	void *new_ptr = allocator->buffer.reallocate(allocator->buffer.context, ptr, oldsz, newsz);
	if (!new_ptr) {
		$heap_trace_failure();
		$thread_alloc_return_error();
	}

	$heap_trace_remove_entry(ptr);

	$heap_trace_add_entry(new_ptr, newsz, source_code_location);
	$thread_alloc_return(new_ptr);
}

void buffer_dealloc(void *ptr, $allocator) {
	$thread_alloc_assert_only_thrd0(dealloc);
	$assert_nonnull(ptr);
	$heap_trace_remove_entry(ptr);
	allocator->buffer.deallocate(allocator->buffer.context, ptr);
}

void buffer_reset($allocator) {
	$thread_alloc_assert_only_thrd0(reset);
	allocator->buffer.reset(allocator->buffer.context);
}

void buffer_destroy($allocator) {
	$thread_alloc_assert_only_thrd0(destroy);
	allocator->buffer.destroy(allocator->buffer.context);
}

void *thread_general_alloc(size_t size, $allocator $source_code_location) {
	return general_alloc(size * thread_count(), allocator, source_code_location);
}

void *
thread_general_resize(void *ptr, size_t size, $allocator $source_code_location) {
	return general_resize(ptr, size * thread_count(), allocator, source_code_location);
}

void *thread_buffer_alloc(size_t size, $allocator $source_code_location) {
	return buffer_alloc(size * thread_count(), allocator, source_code_location);
}

void *
thread_buffer_resize(void *ptr, size_t oldsz, size_t newsz, $allocator $source_code_location) {
	usize tc = thread_count();
	return buffer_resize(ptr, oldsz * tc, newsz * tc, allocator, source_code_location);
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
		$range(0, failure) {
			if (it >= HEAP_TRACE_MAX_FAILURE) break;
			fprintf(
			    fd, "%s:%d:%s: error messages WIP\n", curr->filename, curr->line, curr->funcname);
			curr++;
		}
	}
#endif //  HEAP_TRACE_FAILURE_REPORT
#endif // NDEBUG
}

#define memdup(src, size, mode) memory_dup((src), (size), (mode), allocator)
void *_memdup(void *src, size_t size, Allocator_Mode mode, $allocator) {
	$assert_nonnull(src);
	void *ptr undefined;
	switch (mode) {
	case General_Allocator_Mode: {
		ptr = alloc(size);
		break;
	}
	case Buffer_Allocator_Mode: {
		ptr = balloc(size);
		break;
	}
	}
	if (!ptr) return NULL;
	memcpy(ptr, src, size);
	return ptr;
}
