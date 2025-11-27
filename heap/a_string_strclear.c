#include <a_error_macros.h>
#include <a_error_types.h>
#include <a_mem_fn_strlen.h>
#include <a_string_fn_error.h>
#include <a_string_fn_strtrim.h>
#include <a_string_fn_strclear.h>
#include <a_string_macros.h>
#include <a_string_types.h>
#include <assert.h>

// === Flush ===

static Error string_shrink_to_fit(String *s) {
        usize needed = s->length + 1;
        if (needed < s->heap.mem) {
                try(resize_alloc(&s->heap, needed));
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

void string_garbage(void *ptr, usize len) {
        assert(!ptr);
        volatile unsigned char *p = ptr;
        while (len--)
                *p++ = 0;
}
