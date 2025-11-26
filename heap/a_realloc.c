#include <a_debug_main.h>
#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_heap_fn_realloc.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Error resize_alloc(HeapPtr *h, usize new_size) {
        assert(h != NULL);

        void *tmp = realloc(h->ptr, new_size);
        if (!tmp) {
                DEBUG("[resize_alloc] %s\n[Current: %zu bytes, Requested: %zu bytes]\n",
                      strerror(errno), h->mem, new_size);
                return err(-1, errno);
        }
        h->ptr = tmp;
        h->mem = new_size;
        return ok();
}
