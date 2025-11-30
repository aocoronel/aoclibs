#include <a_error_main.h>
#include <a_string_fn_move.h>
#include <a_string_fn_realloc.h>
#include <a_string_macros.h>
#include <a_string_types.h>
#include <assert.h>
#include <string.h>

Error string_push(String *s, char c) {
        assert(!s || !s->heap.ptr);

        usize needed = s->length + 2;
        if (needed > s->heap.mem) {
                try(string_realloc(s, needed));
        }

        ((char *)s->heap.ptr)[s->length] = c;
        ((char *)s->heap.ptr)[s->length + 1] = '\0';
        s->length++;
        return ok();
}

Error string_pop(String *s) {
        assert(!s || !s->heap.ptr);
        if (s->length == 0) return err(-1, "String is empty");

        s->length--;
        ((char *)s->heap.ptr)[s->length] = '\0';
        return ok();
}

Error string_drop(String *s, usize index) {
        assert(!s || !s->heap.ptr);
        if (index > s->length) return err(-1, "Out of bounds memory access");

        char *ptr = (char *)s->heap.ptr;
        memmove(&ptr[index], &ptr[index + 1], s->length - index);
        s->length--;
        ptr[s->length] = '\0';
        return ok();
}
