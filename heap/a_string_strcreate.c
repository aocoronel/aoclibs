#include <a_string_types.h>
#include <a_error_main.h>
#include <a_heap_main.h>
#include <a_string_fn_error.h>
#include <a_string_macros.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

stringerr string_create(usize cap) {
        if (cap == 0) cap = 16;

        String s = {0};
        Error err;

        try_stringerr(heap_alloc(&s.heap, cap), s);
        s.length = 0;
        ((char *)s.heap.ptr)[0] = '\0';
        return err_stringerr(ok(), s);
}

stringerr string_from(const char *str) {
        assert(str != NULL);

        String s = {0};
        Error err;
        usize len = strlen(str);

        try_stringerr(heap_alloc(&s.heap, len + 1), s);
        memcpy(s.heap.ptr, str, len + 1);
        s.length = len;
        return err_stringerr(ok(), s);
}

void string_free(String *s) {
        if (s == NULL) return;
        heap_free(&s->heap);
        free(s);
}
