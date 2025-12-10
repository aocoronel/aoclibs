#include <aoclibs/common.h>
#include <aoclibs/error.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <string.h>

Err string_push(String *s, char c) {
        assert(!s || !s->heap.ptr);

        usize needed = s->length + 2;
        if (needed > s->heap.cap) {
                Err err = string_realloc(s, needed);
                if (err.code != Ok) return err;
        }

        ((char *)s->heap.ptr)[s->length] = c;
        ((char *)s->heap.ptr)[s->length + 1] = '\0';
        s->length++;
        return ok();
}

Err string_pop(String *s) {
        assert(!s || !s->heap.ptr);
        if (s->length == 0) return eStringIsEmpty;

        s->length--;
        ((char *)s->heap.ptr)[s->length] = '\0';
        return ok();
}

Err string_drop(String *s, usize index) {
        assert(!s || !s->heap.ptr);
        if (index > s->length) return eStringOutOfBounds;

        char *ptr = (char *)s->heap.ptr;
        memmove(&ptr[index], &ptr[index + 1], s->length - index);
        s->length--;
        ptr[s->length] = '\0';
        return ok();
}
