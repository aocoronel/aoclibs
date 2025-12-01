#ifdef DEBUG_HEAP
#ifndef AOCLIBS_DEBUG_HEAP_H
#define AOCLIBS_DEBUG_HEAP_H

#include <stdlib.h>

#define malloc(x) debug_malloc(x, __func__, __FILE__, __LINE__)
#define calloc(n, x) debug_calloc(n, x, __func__, __FILE__, __LINE__)
#define realloc(p, x) debug_realloc(p, x, __func__, __FILE__, __LINE__)
#define free(x) debug_free(x, __func__, __FILE__, __LINE__)

typedef struct MemEntry {
    void *ptr;
    size_t size;
    const char *func;
    const char *file;
    int line;
    struct MemEntry *next;
} MemEntry;

static MemEntry *head = NULL;
static int allocCount = 0;
static int freeCount = 0;

void *debug_malloc(size_t size, const char *func, const char *file, int line);
void *debug_calloc(size_t nmemb, size_t size, const char *func, const char *file, int line);
void *debug_realloc(void *ptr, size_t size, const char *func, const char *file, int line);
void debug_free(void *ptr, const char *func, const char *file, int line);

/*
 * Prints allocation and free count.
 * If there was a leak, print the source of the leak
*/
void print_memory_summary(void);

/*
 * Returns how many leaks found
*/
int count_leaks(void);

#endif
#endif /* DEBUG_HEAP */
