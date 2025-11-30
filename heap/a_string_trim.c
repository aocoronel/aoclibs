#include <a_string_types.h>
#include <a_string_fn_trim.h>
#include <assert.h>
#include <ctype.h>

void string_trim_trailing(String *s) {
        assert(!s || !s->heap.ptr);
        if (s->length == 0) return;

        char *ptr = (char *)s->heap.ptr;
        usize i = s->length;
        while (i > 0 && isspace((unsigned char)ptr[i - 1])) {
                i--;
        }
        s->length = i;
        ptr[i] = '\0';
}
