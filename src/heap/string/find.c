#include <aoclibs/common.h>
#include <aoclibs/error.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <stddef.h>

eusize string_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0) return werr_usize(eStringValNotFound, 0);
        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = 0; i < s->length; i++) {
                if (ptr[i] == c) {
                        return (eusize){ .err = ok(), .value = i };
                }
        }
        return werr_usize(eStringValNotFound, 0);
}

eusize string_reverse_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0) return werr_usize(eStringValNotFound, 0);

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = s->length; i > 0; i--) {
                if (ptr[i] == c) {
                        return (eusize){ .err = ok(), .value = i };
                }
        }
        return werr_usize(eStringValNotFound, 0);
}
