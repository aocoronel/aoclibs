#include <a_heap_fn_free.h>
#include <stdlib.h>

#ifdef DEBUG_HEAP
#include <a_debug_heap.h>
#endif

void heap_free(HeapPtr *h) {
        if (!h || h->ptr == NULL) return;
        free(h->ptr);
        h->ptr = NULL;
        h->mem = 0;
}
