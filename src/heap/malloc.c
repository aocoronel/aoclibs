#include <aoclibs/common.h>
#include <aoclibs/heap/heap.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#include <aoclibs/debug/printfc.h>
#endif

#ifdef DEBUG_HEAP
#include <aoclibs/debug/heap.h>
#endif

Err heap_malloc(Heap *h, usize size) {
        assert(h != NULL);
        void *tmp = malloc(size);
        if (!tmp) {
                ptrace("[heap_malloc] %s [Requested: %zu bytes]\n",
                       strerror(errno), size);
                return eHeapAllocFailed;
        }
        h->ptr = tmp;
        h->cap = size;
        return ok();
}
