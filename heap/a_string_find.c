#include <a_error_macros.h>
#include <a_error_main.h>
#include <a_error_types.h>
#include <a_string_fn_find.h>
#include <a_string_types.h>
#include <assert.h>
#include <stddef.h>

usizeerr string_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0)
                return (usizeerr){
                        .err = { .code = ERROR_FAIL,
                                 .msg = "String s does not have contents" },
                        .value = 0
                };

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = 0; i < s->length; i++) {
                if (ptr[i] == c) {
                        return (usizeerr){ .err = ok(), .value = i };
                }
        }
        return (usizeerr){ .err = { .code = ERROR_FAIL,
                                    .msg = "String s doesn't have c" },
                           .value = 0 };
}

usizeerr string_reverse_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0)
                return (usizeerr){
                        .err = { .code = ERROR_FAIL,
                                 .msg = "String s does not have contents" },
                        .value = 0
                };

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = s->length; i > 0; i--) {
                if (ptr[i] == c) {
                        return (usizeerr){ .err = ok(), .value = i };
                }
        }
        return (usizeerr){ .err = { .code = ERROR_FAIL,
                                    .msg = "String s doesn't have c" },
                           .value = 0 };
}
