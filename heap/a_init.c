#include <a_heap_fn_init.h>
#include <stddef.h>

void heap_init(HeapPtr *h) {
        h->ptr = NULL;
        h->mem = 0;
}
