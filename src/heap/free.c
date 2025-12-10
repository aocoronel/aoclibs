#include <aoclibs/heap/heap.h>
#include <aoclibs/debug/printfc.h>
#include <stdlib.h>

#ifdef DEBUG_HEAP
#include <aoclibs/debug/heap.h>
#endif

void heap_free(Heap *h) {
        if (!h || h->ptr == NULL) return;
        PTRACE("[free] %s [%zu bytes]\n", h->ptr, h->cap);
        free(h->ptr);
        h->ptr = NULL;
        h->cap = 0;
}
