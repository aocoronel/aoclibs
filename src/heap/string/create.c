#include <aoclibs/heap/string.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

string_e string_create(usize cap) {
        if (cap == 0) cap = 16;

        String s = {0};

        Err err = heap_malloc(&s.heap, cap);
        if (err.code != ERR_SUCCESS) return (string_e) {.err = err, .value = s};

        s.length = 0;
        ((char *)s.heap.ptr)[0] = '\0';
        return (string_e) {.err = ok(), .value = s};
}

string_e _string_from(const char *str, usize len) {
        assert(str != NULL);

        String s = {0};

        Err err = heap_malloc(&s.heap, len + 1);
        if (err.code != ERR_SUCCESS) return (string_e) {.err = err, .value = s};

        memcpy(s.heap.ptr, str, len + 1);
        s.length = len;
        return (string_e) {.err = ok(), .value = s};
}

void string_free(String *s) {
        if (s == NULL) return;
        heap_free(&s->heap);
        free(s);
}
