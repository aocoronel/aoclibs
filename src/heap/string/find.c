#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>

eusize string_find(String *_Nonnull s, const char c) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        if (s->length == 0) return werr_usize(ErrValueNotFound, 0);
        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = 0; i < s->length; i++) {
                if (ptr[i] == c) {
                        return (eusize){ .err = ok(), .value = i };
                }
        }
        return werr_usize(ErrValueNotFound, 0);
}

eusize string_reverse_find(String *_Nonnull s, const char c) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        if (s->length == 0) return werr_usize(ErrValueNotFound, 0);

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = s->length; i > 0; i--) {
                if (ptr[i] == c) {
                        return (eusize){ .err = ok(), .value = i };
                }
        }
        return werr_usize(ErrValueNotFound, 0);
}
