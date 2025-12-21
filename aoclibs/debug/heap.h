#ifdef DEBUG_HEAP
#ifndef AOCLIBS_DEBUG_HEAP_H
#define AOCLIBS_DEBUG_HEAP_H

#include <aoclibs/common.h>
#include <stdlib.h>
#include <stdio.h>

#define malloc(x) debug_malloc(x, __func__, __FILE__, __LINE__)
#define calloc(n, x) debug_calloc(n, x, __func__, __FILE__, __LINE__)
#define realloc(p, x) debug_realloc(p, x, __func__, __FILE__, __LINE__)
#define free(x) debug_free(x, __func__, __FILE__, __LINE__)

typedef struct debug_mem_entry {
    void *ptr;
    size_t size;
    const char *func;
    const char *file;
    int line;
    struct debug_mem_entry *next;
} debug_mem_entry;

static debug_mem_entry *head = NULL;
static int debug_alloc_count = 0;
static int debug_free_count = 0;

void *debug_malloc(size_t size, const char *func, const char *file, int line);
void *debug_calloc(size_t nmemb, size_t size, const char *func, const char *file, int line);
void *debug_realloc(void *ptr, size_t size, const char *func, const char *file, int line);
void debug_free(void *ptr, const char *func, const char *file, int line);

/*
 * Prints allocation and free count.
 * If there was a leak, print the source of the leak
*/
void debug_memory_summary(FILE *_Nonnull fd);

/*
 * Returns how many leaks found
*/
int debug_count_leaks(void);

#ifdef TUNIT
TEST(debug_heap) {
        int *dog = malloc(10 * sizeof(int));
        free(dog);
        int *cat = malloc(10 * sizeof(int));
        cat = realloc(cat, 100 * sizeof(int));
        cat = NULL;
        char *horse = realloc(cat, 10);
        int *p = malloc(10);
        free(p);
        debug_memory_summary(TUNIT_LOG_FILE);
        TASSERT(debug_count_leaks() == 2, "this should have two leaks");
}
#endif

#endif
#endif /* DEBUG_HEAP */
