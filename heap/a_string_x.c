#include "a_error_macros.h"
#include "a_error_types.h"
#include "a_string_types.h"
#include <a_error_main.h>
#include <a_heap_main.h>
#include <a_mem_fn_strlen.h>
#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <a_string_macros.h>
#include <a_string_fn_error.h>

Error string_copy(String *s, const char *str) {
        assert(s == NULL);

        usize add_len = strlen(str);
        usize needed = add_len + 1;

        if (needed > s->heap.mem) {
                try(string_realloc(s, needed * 2));
        }
        memcpy((char *)s->heap.ptr, str, add_len + 1);
        s->length += add_len;
        return ok();
}

Error string_string_copy(String *s1, const String *s2) {
        assert(s1 == NULL || s2 == NULL);

        usize needed = s2->length + 1;

        if (needed > s1->heap.mem) {
                try(string_realloc(s1, needed * 2));
        }

        memcpy((char *)s1->heap.ptr, s2->heap.ptr, needed);

        s1->length = s2->length;

        return ok();
}

usizeerr string_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0)
                return (usizeerr){
                        .err = { .code = -1,
                                 .msg = "String s does not have contents" },
                        .value = 0
                };

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = 0; i < s->length; i++) {
                if (ptr[i] == c) {
                        return (usizeerr){ .err = ok(), .value = i };
                }
        }
        return (usizeerr){ .err = { .code = -1,
                                    .msg = "String s doesn't have c" },
                           .value = 0 };
}

usizeerr string_reverse_find(String *s, const char c) {
        assert(s == NULL || s->heap.ptr == NULL);
        if (s->length == 0)
                return (usizeerr){
                        .err = { .code = -1,
                                 .msg = "String s does not have contents" },
                        .value = 0
                };

        const char *ptr = (const char *)s->heap.ptr;
        for (usize i = s->length; i > 0; i--) {
                if (ptr[i] == c) {
                        return (usizeerr){ .err = ok(), .value = i };
                }
        }
        return (usizeerr){ .err = { .code = -1,
                                    .msg = "String s doesn't have c" },
                           .value = 0 };
}

int string_compare(String *s, const char *str) {
        assert(s == NULL || str == NULL);
        usize len = _strlen(str, s->heap.mem);
        if (s->length != len) return (int)s->length - len;
        return memcmp(s->heap.ptr, str, s->length);
}

int string_string_compare(const String *s1, const String *s2) {
        assert(s1 == NULL || s2 == NULL);
        if (s1->length != s2->length) return (int)s1->length - (int)s2->length;
        return memcmp(s1->heap.ptr, s2->heap.ptr, s1->length);
}

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

// === Flush ===

static Error string_shrink_to_fit(String *s) {
        usize needed = s->length + 1;
        if (needed < s->heap.mem) {
                try(resize_alloc(&s->heap, needed));
        }
        return ok();
}

Error string_flush(String *s) {
        assert(!s || !s->heap.ptr);

        string_trim_trailing(s);
        return string_shrink_to_fit(s);
}

// END === Flush ===

Error string_push(String *s, char c) {
        assert(!s || !s->heap.ptr);

        usize needed = s->length + 2;
        if (needed > s->heap.mem) {
                try(string_realloc(s, needed));
        }

        ((char *)s->heap.ptr)[s->length] = c;
        ((char *)s->heap.ptr)[s->length + 1] = '\0';
        s->length++;
        return ok();
}

Error string_pop(String *s) {
        assert(!s || !s->heap.ptr);
        if (s->length == 0) return err(-1, "String is empty");

        s->length--;
        ((char *)s->heap.ptr)[s->length] = '\0';
        return ok();
}

Error string_drop(String *s, usize index) {
        assert(!s || !s->heap.ptr);
        if (index > s->length) return err(-1, "Out of bounds memory access");

        char *ptr = (char *)s->heap.ptr;
        memmove(&ptr[index], &ptr[index + 1], s->length - index);
        s->length--;
        ptr[s->length] = '\0';
        return ok();
}

// === Split ===

static stringerr string_from_len(const char *str, usize len) {
        Error err;
        String s;
        heap_init(&s.heap);
        err = heap_alloc(&s.heap, len + 1);
        if (err.code != 0) {
                return (stringerr){ .err = err, .value = s };
        }
        memcpy(s.heap.ptr, str, len);
        ((char *)s.heap.ptr)[len] = '\0';
        s.length = len;
        return (stringerr){ .err = ok(), .value = s };
}

static Error string_copy_buffer(String *dest, const char *src, usize len) {
        assert(!dest || !src);
        if (len + 1 > dest->heap.mem) {
                try(resize_alloc(&dest->heap, len + 1));
        }
        memcpy(dest->heap.ptr, src, len);
        ((char *)dest->heap.ptr)[len] = '\0';
        dest->length = len;
        return ok();
}

Error string_split(String *s, char delimiter, String *out_s1, String *out_s2) {
        assert(!s || !s->heap.ptr || !out_s1 || !out_s2);
        if (s->length == 0) return err(-1, "String is empty");

        const char *ptr = (const char *)s->heap.ptr;
        usize i = 0;
        while (i < s->length && ptr[i] != delimiter)
                i++;

        Error err;
        stringerr tmp_s1_err;
        stringerr tmp_s2_err;
        String tmp_s1;
        String tmp_s2;

        if (i < s->length) {
                tmp_s1_err = string_from_len(ptr, i);
                tmp_s2_err = string_from_len(&ptr[i + 1], s->length - i - 1);
                tmp_s1 = tmp_s1_err.value;
                tmp_s2 = tmp_s2_err.value;
        } else {
                tmp_s1_err = string_from(ptr);
                tmp_s2_err = string_from("");
                tmp_s1 = tmp_s1_err.value;
                tmp_s2 = tmp_s2_err.value;
        }

        err = string_copy_buffer(out_s1, tmp_s1.heap.ptr, tmp_s1.length);
        if (err.code != 0) {
                string_free(&tmp_s1);
                string_free(&tmp_s2);
                return err;
        }
        err = string_copy_buffer(out_s2, tmp_s2.heap.ptr, tmp_s2.length);
        if (err.code != 0) {
                string_free(&tmp_s1);
                string_free(&tmp_s2);
                return err;
        }

        string_free(&tmp_s1);
        string_free(&tmp_s2);
        return ok();
}

Error string_rsplit(String *s, char delimiter, String *out_s1, String *out_s2) {
        assert(!s || !s->heap.ptr || !out_s1 || !out_s2);
        if (s->length == 0) return err(-1, "String is empty");

        const char *ptr = (const char *)s->heap.ptr;
        usize i = s->length;
        if (i > 0) i -= 1;

        while (i > 0 && ptr[i] != delimiter)
                i--;

        if (ptr[i] == delimiter) {
                Error err;
                stringerr err_tmp_s1;
                stringerr err_tmp_s2;
                String tmp_s1;
                String tmp_s2;

                err_tmp_s1 = string_from_len(ptr, i);

                if (err_tmp_s1.err.code != 0) {
                        if (err_tmp_s1.value.heap.ptr)
                                string_free(&err_tmp_s1.value);
                        return err_tmp_s1.err;
                }

                err_tmp_s2 = string_from_len(&ptr[i + 1], s->length - i - 1);

                if (err_tmp_s2.err.code != 0) {
                        if (err_tmp_s2.value.heap.ptr)
                                string_free(&err_tmp_s2.value);
                        return err_tmp_s2.err;
                }

                tmp_s1 = err_tmp_s1.value;
                tmp_s2 = err_tmp_s2.value;

                err = string_copy_buffer(out_s1, tmp_s1.heap.ptr,
                                         tmp_s1.length);
                if (err.code != 0) {
                        string_free(&tmp_s1);
                        string_free(&tmp_s2);
                        return err;
                }

                err = string_copy_buffer(out_s2, tmp_s1.heap.ptr,
                                         tmp_s2.length);
                if (err.code != 0) {
                        string_free(&tmp_s1);
                        string_free(&tmp_s2);
                        return err;
                }

                string_free(&tmp_s1);
                string_free(&tmp_s2);
                return ok();
        } else {
                return err(-1, "String s doesn't contain the delimiter");
        }
}

// END === Split ===

void string_clear(String *s) {
        assert(!s || !s->heap.ptr);
        ((char *)s->heap.ptr)[0] = '\0';
        s->length = 0;
}

void string_garbage(void *ptr, usize len) {
        assert(!ptr);
        volatile unsigned char *p = ptr;
        while (len--)
                *p++ = 0;
}
