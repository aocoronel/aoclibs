#ifdef DEBUG_HEAP
#include <aoclibs/common.h>
#include <aoclibs/debug/heap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *(*std_malloc)(size_t) = NULL;
static void *(*std_calloc)(size_t, size_t) = NULL;
static void *(*std_realloc)(void *, size_t) = NULL;
static void (*std_free)(void *) = NULL;

static void init_std_functions(void) {
        if (std_malloc) return;
        std_malloc = malloc;
        std_calloc = calloc;
        std_realloc = realloc;
        std_free = free;
}

static void add_entry(void *ptr, size_t size, const char *func,
                      const char *file, int line) {
        init_std_functions();
        debug_mem_entry *entry =
                (debug_mem_entry *)std_malloc(sizeof(debug_mem_entry));
        if (!entry) return;

        entry->ptr = ptr;
        entry->size = size;
        entry->func = func;
        entry->file = file;
        entry->line = line;
        entry->next = head;
        head = entry;
        debug_alloc_count++;
}

static void remove_entry(void *ptr) {
        init_std_functions();
        debug_mem_entry **curr = &head;
        while (*curr) {
                if ((*curr)->ptr == ptr) {
                        debug_mem_entry *to_free = *curr;
                        *curr = to_free->next;
                        std_free(to_free);
                        debug_free_count++;
                        return;
                }
                curr = &(*curr)->next;
        }
}

void *debug_malloc(size_t size, const char *func, const char *file, int line) {
        init_std_functions();
        void *ptr = std_malloc(size);
        if (ptr) {
                add_entry(ptr, size, func, file, line);
        }
        return ptr;
}

void *debug_calloc(size_t nmemb, size_t size, const char *func,
                   const char *file, int line) {
        init_std_functions();
        void *ptr = std_calloc(nmemb, size);
        if (ptr) {
                add_entry(ptr, nmemb * size, func, file, line);
        }
        return ptr;
}

void *debug_realloc(void *ptr, size_t size, const char *func, const char *file,
                    int line) {
        init_std_functions();
        if (ptr == NULL) {
                void *new_ptr = std_malloc(size);
                if (new_ptr) add_entry(new_ptr, size, func, file, line);
                return new_ptr;
        }

        debug_mem_entry **entry_ptr = &head;
        while (*entry_ptr && (*entry_ptr)->ptr != ptr) {
                entry_ptr = &(*entry_ptr)->next;
        }

        void *new_ptr = std_realloc(ptr, size);
        if (new_ptr && *entry_ptr) {
                (*entry_ptr)->ptr = new_ptr;
                (*entry_ptr)->size = size;
                (*entry_ptr)->func = func;
                (*entry_ptr)->file = file;
                (*entry_ptr)->line = line;
        } else if (new_ptr) {
                add_entry(new_ptr, size, func, file, line);
        }
        return new_ptr;
}

void debug_free(void *ptr, const char *func, const char *file, int line) {
        if (!ptr) return;
        remove_entry(ptr);
        init_std_functions();
        std_free(ptr);
}

int debug_count_leaks(void) {
        return debug_alloc_count - debug_free_count;
}

void debug_memory_summary(FILE *_Nonnull fd) {
        debug_mem_entry *curr = head;
        int leaks_found = 0;
        fprintf(fd, "===== Memory Summary Report =====\n");
        fprintf(fd, "Total allocations: %d\n", debug_alloc_count);
        fprintf(fd, "Total frees: %d\n", debug_free_count);
        fprintf(fd, "Active allocations (leaks): %d\n",
               debug_alloc_count - debug_free_count);
        while (curr) {
                fprintf(fd, "%zu bytes at %s() in %s:%d (ptr: %p)\n", curr->size,
                       curr->func, curr->file, curr->line, curr->ptr);
                leaks_found += curr->size;
                curr = curr->next;
        }
        if (debug_alloc_count == debug_free_count) {
                fprintf(fd, "No memory leaks detected.\n");
        }
}
#endif /* DEBUG_HEAP */
