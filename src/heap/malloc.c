#include <aoclibs/common.h>
#include <aoclibs/heap/heap.h>
#include <aoclibs/assert.h>
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
        ASSERT(h != NULL, "pointer cannot be NULL");
        ASSERT(size > 0, "size cannot be negative");

        void *tmp = malloc(size);
        if (!tmp) {
                PTRACE("[heap_malloc] %s [Requested: %zu bytes]\n",
                       strerror(errno), size);
                return werr(errno, strerror(errno));
        }
        h->ptr = tmp;
        h->cap = size;
        return ok();
}
