#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

eString string_create(usize cap) {
        if (cap == 0) cap = 16;

        String s = { 0 };

        Err err = heap_malloc(&s.heap, cap);
        if (err.code != Ok) return werr_string(err, s);

        s.length = 0;
        ((char *)s.heap.ptr)[0] = '\0';
        return (eString){ .err = ok(), .value = s };
}

eString _string_from(const char *str, usize len) {
        assert(str != NULL);

        String s = { 0 };

        Err err = heap_malloc(&s.heap, len + 1);
        if (err.code != Ok) return werr_string(err, s);

        memcpy(s.heap.ptr, str, len + 1);
        s.length = len;
        return (eString){ .err = ok(), .value = s };
}

void string_free(String *s) {
        if (s == NULL) return;
        heap_free(&s->heap);
        free(s);
}
