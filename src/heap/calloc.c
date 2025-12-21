#include <aoclibs/common.h>
#include <aoclibs/printfc.h>
#include <aoclibs/heap/heap.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG_HEAP
#include <aoclibs/debug/heap.h>
#endif

Err heap_calloc(Heap *_Nonnull h, usize count, usize size) {
        ASSERT(h != NULL, "%s", "pointer cannot be NULL");
        ASSERT(size > 0, "%s", "size cannot be negative");

        void *tmp = calloc(count, size);
        if (!tmp) {
                PTRACE("[heap_calloc] %s [Requested: %zu bytes]\n",
                       strerror(errno), count * size);
                return ErrErrno;
        }
        h->ptr = tmp;
        h->cap = count * size;
        return ok();
}
