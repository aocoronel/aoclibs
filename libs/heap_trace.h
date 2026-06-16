#ifndef AOCLIBS_HEAP_TRACE_H_
#define AOCLIBS_HEAP_TRACE_H_

#include <stdio.h>
#include <stdlib.h>

// These functions will be ignored by HEAP_TRACE
void *no_debug_malloc(size_t size);
void *no_debug_calloc(size_t nmeb, size_t size);
void *no_debug_realloc(void *ptr, size_t size);
void no_debug_free(void *ptr);

#ifdef HEAP_TRACE

// Enable Heap Tracer :: cc -DHEAP_TRACE

void *no_debug_malloc(size_t size) {
	return malloc(size);
}

void *no_debug_calloc(size_t nmeb, size_t size) {
	return calloc(nmeb, size);
}

void *no_debug_realloc(void *ptr, size_t size) {
	return realloc(ptr, size);
}

void no_debug_free(void *ptr) {
	free(ptr);
}

/*
 * heap_count_leaks :: Returns how many leaks found
*/
#define heap_count_leaks __trace_alloc_count - __trace_free_count

/*
 * Prints allocation and free count.
 * If there was a leak, print the source of the leak
*/
static inline void heap_trace_summary(FILE *fd);

typedef struct __heap_trace_entry {
	void *ptr;
	size_t size;
	const char *func;
	const char *file;
	int line;
	struct __heap_trace_entry *next;
} __heap_trace_entry;

/*
 * Replaces allocators with custom debug ones
*/
static void *(*__trace_std_malloc)(size_t) = NULL;
static void *(*__trace_std_calloc)(size_t, size_t) = NULL;
static void *(*__trace_std_realloc)(void *, size_t) = NULL;
static void (*__trace_std_free)(void *) = NULL;

static void __init_trace_std_alloc_fn(void) {
	if (__trace_std_malloc) return;
	__trace_std_malloc = malloc;
	__trace_std_calloc = calloc;
	__trace_std_realloc = realloc;
	__trace_std_free = free;
}

static __heap_trace_entry *__entry_head = NULL;
static int __trace_alloc_count = 0;
static int __trace_free_count = 0;

static inline void
__trace_add_entry(void *ptr, size_t size, const char *func, const char *file, int line) {
	__init_trace_std_alloc_fn();
	__heap_trace_entry *entry =
			(__heap_trace_entry *)__trace_std_malloc(sizeof(__heap_trace_entry));
	if (!entry) return;

	entry->ptr = ptr;
	entry->size = size;
	entry->func = func;
	entry->file = file;
	entry->line = line;
	entry->next = __entry_head;
	__entry_head = entry;
	__trace_alloc_count++;
}

static inline void __trace_remove_entry(void *ptr) {
	__init_trace_std_alloc_fn();
	__heap_trace_entry **curr = &__entry_head;
	while (*curr) {
		if ((*curr)->ptr == ptr) {
			__heap_trace_entry *to_free = *curr;
			*curr = to_free->next;
			__trace_std_free(to_free);
			__trace_free_count++;
			return;
		}
		curr = &(*curr)->next;
	}
}

static inline void *__trace_malloc(size_t size, const char *func, const char *file, int line) {
	__init_trace_std_alloc_fn();
	void *ptr = __trace_std_malloc(size);
	if (ptr) {
		__trace_add_entry(ptr, size, func, file, line);
	}
	return ptr;
}

static inline void *
__trace_calloc(size_t nmemb, size_t size, const char *func, const char *file, int line) {
	__init_trace_std_alloc_fn();
	void *ptr = __trace_std_calloc(nmemb, size);
	if (ptr) {
		__trace_add_entry(ptr, nmemb * size, func, file, line);
	}
	return ptr;
}

static inline void *
__trace_realloc(void *ptr, size_t size, const char *func, const char *file, int line) {
	__init_trace_std_alloc_fn();
	if (ptr == NULL) {
		void *new_ptr = __trace_std_malloc(size);
		if (new_ptr) __trace_add_entry(new_ptr, size, func, file, line);
		return new_ptr;
	}

	__heap_trace_entry **entry_ptr = &__entry_head;
	while (*entry_ptr && (*entry_ptr)->ptr != ptr) {
		entry_ptr = &(*entry_ptr)->next;
	}

	void *new_ptr = __trace_std_realloc(ptr, size);
	if (new_ptr && *entry_ptr) {
		(*entry_ptr)->ptr = new_ptr;
		(*entry_ptr)->size = size;
		(*entry_ptr)->func = func;
		(*entry_ptr)->file = file;
		(*entry_ptr)->line = line;
	} else if (new_ptr) {
		__trace_add_entry(new_ptr, size, func, file, line);
	}
	return new_ptr;
}

static inline void __trace_free(void *ptr, const char *func, const char *file, int line) {
	if (!ptr) return;
	__trace_remove_entry(ptr);
	__init_trace_std_alloc_fn();
	__trace_std_free(ptr);
}

static inline void heap_trace_summary(FILE *fd) {
	__heap_trace_entry *curr = __entry_head;
	int leaks_found = 0;
	fprintf(fd, "===== Memory Summary Report =====\n");
	fprintf(fd, "Total allocations: %d\n", __trace_alloc_count);
	fprintf(fd, "Total frees: %d\n", __trace_free_count);
	fprintf(fd, "Active allocations (leaks): %d\n", __trace_alloc_count - __trace_free_count);
	while (curr) {
		fprintf(fd,
				"%zu bytes at %s() in %s:%d (ptr: %p)\n",
				curr->size,
				curr->func,
				curr->file,
				curr->line,
				curr->ptr);
		leaks_found += curr->size;
		curr = curr->next;
	}
	if (__trace_alloc_count == __trace_free_count) {
		fprintf(fd, "No memory leaks detected.\n");
	}
}

#define malloc(x) __trace_malloc(x, __func__, __FILE__, __LINE__)
#define calloc(n, x) __trace_calloc(n, x, __func__, __FILE__, __LINE__)
#define realloc(p, x) __trace_realloc(p, x, __func__, __FILE__, __LINE__)
#define free(x) __trace_free(x, __func__, __FILE__, __LINE__)

#endif

#endif // AOCLIBS_HEAP_TRACE_H_
