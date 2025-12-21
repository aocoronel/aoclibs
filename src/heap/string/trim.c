#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <ctype.h>

void string_trim_trailing(String *_Nonnull s) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        if (s->length == 0) return;

        char *ptr = (char *)s->heap.ptr;
        usize i = s->length;
        while (i > 0 && isspace((unsigned char)ptr[i - 1])) {
                i--;
        }
        s->length = i;
        ptr[i] = '\0';
}
