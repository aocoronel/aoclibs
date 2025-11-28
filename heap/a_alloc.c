#include <a_debug_main.h>
#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_heap_fn_alloc.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

Error heap_alloc(HeapPtr *h, usize size) {
        assert(h != NULL);
        void *tmp = malloc(size);
        if (!tmp) {
                DEBUG("[heap_alloc] %s\n[Requested: %zu bytes]\n",
                      strerror(errno), size);
                return err(-1, errno);
        }
        h->ptr = tmp;
        h->mem = size;
        return ok();
}
