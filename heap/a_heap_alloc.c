#include <a_debug_main.h>
#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_heap_fn_alloc.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifndef NDEBUG
#include <stdio.h>
#endif

#ifdef DEBUG_HEAP
#include <a_debug_heap.h>
#endif

Error heap_malloc(HeapPtr *h, usize size) {
        assert(h != NULL);
        void *tmp = malloc(size);
        if (!tmp) {
                PDEBUG("[heap_malloc] %s\n[Requested: %zu bytes]\n",
                      strerror(errno), size);
                return err(errno, "Malloc failed");
        }
        h->ptr = tmp;
        h->mem = size;
        return ok();
}
