#include <aoclibs/heap/heap.h>
#include <aoclibs/printfc.h>
#include <stdlib.h>
#include <aoclibs/assert.h>

#ifdef DEBUG_HEAP
#include <aoclibs/debug/heap.h>
#endif

void heap_free(Heap *_Nonnull h) {
        ASSERT(h != NULL || h->ptr == NULL, "%s", "double free attempt");
        PTRACE("[free] %s [%zu bytes]\n", h->ptr, h->cap);
        free(h->ptr);
        h->ptr = NULL;
        h->cap = 0;
}
