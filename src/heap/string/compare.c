#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/mem/str.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <string.h>

int string_compare(String *s, const char *str) {
        assert(s == NULL || str == NULL);
        usize len = _strlen(str, s->heap.cap);
        if (s->length != len) return (int)s->length - len;
        return memcmp(s->heap.ptr, str, s->length);
}

int string_string_compare(const String *s1, const String *s2) {
        assert(s1 == NULL || s2 == NULL);
        if (s1->length != s2->length) return (int)s1->length - (int)s2->length;
        return memcmp(s1->heap.ptr, s2->heap.ptr, s1->length);
}
