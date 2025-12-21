#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <string.h>

Err string_push(String *_Nonnull s, char c) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

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

Err string_pop(String *_Nonnull s) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        if (s->length == 0) return ErrElementIsEmpty;

        s->length--;
        ((char *)s->heap.ptr)[s->length] = '\0';
        return ok();
}

Err string_drop(String *_Nonnull s, usize index) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");
        ASSERT(index > 0, "%s", "index \"%d\" cannot be negative", index);

        if (index > s->length) return ErrOutOfBounds;

        char *ptr = (char *)s->heap.ptr;
        memmove(&ptr[index], &ptr[index + 1], s->length - index);
        s->length--;
        ptr[s->length] = '\0';
        return ok();
}
