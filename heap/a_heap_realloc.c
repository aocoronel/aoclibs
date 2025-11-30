#include <a_debug_main.h>
#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_heap_fn_realloc.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_HEAP
#include <a_debug_heap.h>
#endif

Error heap_realloc(HeapPtr *h, usize new_size) {
        assert(h != NULL);

        void *tmp = realloc(h->ptr, new_size);
        if (!tmp) {
                DEBUG("[heap_realloc] %s\n[Current: %zu bytes, Requested: %zu bytes]\n",
                      strerror(errno), h->mem, new_size);
                return err(errno, "Realloc failed");
        }
        h->ptr = tmp;
        h->mem = new_size;
        return ok();
}
