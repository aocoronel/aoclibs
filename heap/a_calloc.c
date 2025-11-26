#include <a_debug_main.h>
#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_heap_fn_calloc.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Error heap_clean_alloc(HeapPtr *h, usize count, usize size) {
        assert(h != NULL);
        void *tmp = calloc(count, size);
        if (!tmp) {
                DEBUG("[heap_clean_alloc] %s\n[Requested: %zu bytes]\n",
                      strerror(errno), count * size);
                return err(-1, errno);
        }
        h->ptr = tmp;
        h->mem = count * size;
        return ok();
}
