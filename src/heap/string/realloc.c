#include <aoclibs/error.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/int.h>
#include <stddef.h>

Err string_realloc(String *s, usize capacity) {
        return heap_realloc(&s->heap, capacity);
}
