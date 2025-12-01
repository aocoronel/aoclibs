#ifdef DEBUG_HEAP

#include <a_debug_heap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *(*std_malloc)(size_t) = NULL;
static void *(*std_calloc)(size_t, size_t) = NULL;
static void *(*std_realloc)(void *, size_t) = NULL;
static void (*std_free)(void *) = NULL;

static void init_std_functions(void) {
        if (!std_malloc) {
                std_malloc = malloc; // Use current malloc (system's)
                std_calloc = calloc;
                std_realloc = realloc;
                std_free = free;
        }
}

static void add_entry(void *ptr, size_t size, const char *func, const char *file,
               int line) {
        init_std_functions();
        MemEntry *entry = (MemEntry *)std_malloc(sizeof(MemEntry));
        if (!entry) return;

        entry->ptr = ptr;
        entry->size = size;
        entry->func = func;
        entry->file = file;
        entry->line = line;
        entry->next = head;
        head = entry;
        allocCount++;
}

static void remove_entry(void *ptr) {
        init_std_functions();
        MemEntry **curr = &head;
        while (*curr) {
                if ((*curr)->ptr == ptr) {
                        MemEntry *to_free = *curr;
                        *curr = to_free->next;
                        std_free(to_free);
                        freeCount++;
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

        MemEntry **entry_ptr = &head;
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

int count_leaks(void) {
        return allocCount - freeCount;
}

void print_memory_summary(void) {
        MemEntry *curr = head;
        int leaks_found = 0;
        printf("===== Memory Summary Report =====\n");
        printf("Total allocations: %d\n", allocCount);
        printf("Total frees: %d\n", freeCount);
        printf("Active allocations (leaks): %d\n", allocCount - freeCount);
        while (curr) {
                printf("%zu bytes at %s() in %s:%d (ptr: %p)\n", curr->size,
                       curr->func, curr->file, curr->line, curr->ptr);
                leaks_found += curr->size;
                curr = curr->next;
        }
        if (allocCount == freeCount) {
                printf("No memory leaks detected.\n");
        }
}
#endif /* DEBUG_HEAP */
