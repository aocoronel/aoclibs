#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>

Err string_realloc(String *_Nonnull s, usize capacity) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        return heap_realloc(&s->heap, capacity);
}
