#include <a_error_fn_ok.h>
#include <a_error_macros.h>
#include <a_heap_fn_alloc.h>
#include <a_heap_fn_realloc.h>
#include <a_string_types.h>
#include <stddef.h>

Error string_realloc(String *s, usize capacity) {
        if (s != NULL) try(heap_malloc(&s->heap, capacity));
        try(heap_realloc(&s->heap, capacity));
        return ok();
}
