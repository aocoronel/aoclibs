#include <aoclibs/common.h>
#include <aoclibs/error.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <stddef.h>

usize_e string_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0)
                return (usize_e){
                        .err = { .code = ERR_FAIL,
                                 .msg = "String s does not have contents" },
                        .value = 0
                };

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = 0; i < s->length; i++) {
                if (ptr[i] == c) {
                        return (usize_e){ .err = ok(), .value = i };
                }
        }
        return (usize_e){ .err = { .code = ERR_FAIL,
                                   .msg = "String s doesn't have c" },
                          .value = 0 };
}

usize_e string_reverse_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0)
                return (usize_e){
                        .err = { .code = ERR_FAIL,
                                 .msg = "String s does not have contents" },
                        .value = 0
                };

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = s->length; i > 0; i--) {
                if (ptr[i] == c) {
                        return (usize_e){ .err = ok(), .value = i };
                }
        }
        return (usize_e){ .err = { .code = ERR_FAIL,
                                   .msg = "String s doesn't have c" },
                          .value = 0 };
}
