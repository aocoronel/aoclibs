#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ALLOC_DEBUG
#define ALLOC_DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define ALLOC_DEBUG_PRINT(...)
#endif

typedef struct HeapPtr {
        void *ptr;
        size_t mem;
} HeapPtr;

void heap_init(HeapPtr *h) {
        h->ptr = NULL;
        h->mem = 0;
}

int heap_alloc(HeapPtr *h, size_t size) {
        if (!h) return 0;
        void *tmp = malloc(size);
        if (!tmp) {
                ALLOC_DEBUG_PRINT("[heap_alloc] %s\n[Requested: %zu bytes]\n",
                                  strerror(errno), size);
                return 0;
        }
        h->ptr = tmp;
        h->mem = size;
        return 1;
}

int heap_clean_alloc(HeapPtr *h, size_t count, size_t size) {
        if (!h) return 0;
        void *tmp = calloc(count, size);
        if (!tmp) {
                ALLOC_DEBUG_PRINT(
                        "[heap_clean_alloc] %s\n[Requested: %zu bytes]\n",
                        strerror(errno), count * size);
                return 0;
        }
        h->ptr = tmp;
        h->mem = count * size;
        return 1;
}

int resize_alloc(HeapPtr *h, size_t new_size) {
        if (!h) return 0;

        void *tmp = realloc(h->ptr, new_size);
        if (!tmp) {
                ALLOC_DEBUG_PRINT(
                        "[resize_alloc] %s\n[Current: %zu bytes, Requested: %zu bytes]\n",
                        strerror(errno), h->mem, new_size);
                return 0;
        }
        h->ptr = tmp;
        h->mem = new_size;
        return 1;
}

void heap_free(HeapPtr *h) {
        if (!h || h->ptr == NULL) return;
        free(h->ptr);
        h->ptr = NULL;
        h->mem = 0;
}
