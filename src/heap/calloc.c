#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <aoclibs/heap/heap.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#include <aoclibs/debug/printfc.h>
#endif

#ifdef DEBUG_HEAP
#include <a_debug_heap.h>
#endif

Err heap_calloc(Heap *h, usize count, usize size) {
        assert(h != NULL);
        void *tmp = calloc(count, size);
        if (!tmp) {
                ptrace("[heap_calloc] %s [Requested: %zu bytes]\n",
                      strerror(errno), count * size);
                return err(errno, "calloc failed");
        }
        h->ptr = tmp;
        h->cap = count * size;
        return ok();
}
