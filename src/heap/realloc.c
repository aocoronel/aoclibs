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

Err heap_realloc(Heap *h, usize new_size) {
        assert(h != NULL);

        void *tmp = realloc(h->ptr, new_size);
        if (!tmp) {
                PTRACE("[heap_realloc] %s [Current: %zu bytes, Requested: %zu bytes]\n",
                       strerror(errno), h->cap, new_size);
                return eHeapAllocFailed;
        }
        h->ptr = tmp;
        h->cap = new_size;
        return ok();
}
