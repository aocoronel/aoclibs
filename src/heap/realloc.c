#include <aoclibs/common.h>
#include <aoclibs/printfc.h>
#include <aoclibs/heap/heap.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_HEAP
#include <aoclibs/debug/heap.h>
#endif

Err heap_realloc(Heap *_Nonnull h, usize new_size) {
        ASSERT(h != NULL, "%s", "pointer cannot be NULL");
        ASSERT(new_size > 0, "%s", "size cannot be negative");

        void *tmp = realloc(h->ptr, new_size);
        if (!tmp) {
                PTRACE("[heap_realloc] %s [Current: %zu bytes, Requested: %zu bytes]\n",
                       strerror(errno), h->cap, new_size);
                return ErrErrno;
        }
        h->ptr = tmp;
        h->cap = new_size;
        return ok();
}
