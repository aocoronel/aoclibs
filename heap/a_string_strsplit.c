#include <a_error_main.h>
#include <a_heap_main.h>
#include <a_string_fn_error.h>
#include <a_string_fn_strcreate.h>
#include <a_string_fn_strplit.h>
#include <a_string_macros.h>
#include <a_string_types.h>
#include <assert.h>
#include <string.h>

// === Split ===

static stringerr string_from_len(const char *str, usize len) {
        Error err;
        String s = {0};

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
