#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/mem/str.h>
#include <string.h>

int string_equal(String *_Nonnull s, const char *_Nonnull str,
                 const usize str_len) {
        ASSERT(s != NULL && str != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");
        ASSERT(str_len > 0, "%s", "string length \"%d\" cannot be negative",
               str_len);

        if (s->length != str_len) return (int)s->length - str_len;
        return memcmp(s->heap.ptr, str, s->length);
}

int string_string_equal(const String *_Nonnull s1, const String *_Nonnull s2) {
        ASSERT(s1 != NULL && s2 != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s1->heap.ptr != NULL && s2->heap.ptr != NULL, "%s",
               "string heap pointer is NULL");
        if (s1->length != s2->length) return (int)s1->length - (int)s2->length;
        return memcmp(s1->heap.ptr, s2->heap.ptr, s1->length);
}
