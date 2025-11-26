#include <a_error_fn_ok.h>
#include <a_error_macros.h>
#include <a_heap_fn_realloc.h>
#include <a_string_types.h>
#include <assert.h>
#include <stddef.h>

Error string_realloc(String *s, usize capacity) {
        assert(s != NULL);
        try(resize_alloc(&s->heap, capacity));
        return ok();
}
