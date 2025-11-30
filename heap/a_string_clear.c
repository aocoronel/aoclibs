#include <a_error_macros.h>
#include <a_error_types.h>
#include <a_mem_fn_strlen.h>
#include <a_string_fn_trim.h>
#include <a_string_fn_clear.h>
#include <a_string_macros.h>
#include <a_string_types.h>
#include <assert.h>

// === Flush ===

static Error string_shrink_to_fit(String *s) {
        usize needed = s->length + 1;
        if (needed < s->heap.mem) {
                try(heap_realloc(&s->heap, needed));
        }
        return ok();
}

Error string_flush(String *s) {
        assert(!s || !s->heap.ptr);

        string_trim_trailing(s);
        return string_shrink_to_fit(s);
}

// END === Flush ===

void string_clear(String *s) {
        assert(!s || !s->heap.ptr);
        ((char *)s->heap.ptr)[0] = '\0';
        s->length = 0;
}

void string_garbage(String *s, usize len) {
        assert(!s->heap.ptr);
        volatile unsigned char *p = s->heap.ptr;
        while (len--)
                *p++ = 0;
}
