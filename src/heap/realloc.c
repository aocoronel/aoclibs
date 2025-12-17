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

Err heap_realloc(Heap *h, usize new_size) {
        ASSERT(h != NULL, "pointer cannot be NULL");
        ASSERT(new_size > 0, "size cannot be negative");

        void *tmp = realloc(h->ptr, new_size);
        if (!tmp) {
                PTRACE("[heap_realloc] %s [Current: %zu bytes, Requested: %zu bytes]\n",
                       strerror(errno), h->cap, new_size);
                return werr(errno, strerror(errno));
        }
        h->ptr = tmp;
        h->cap = new_size;
        return ok();
}
