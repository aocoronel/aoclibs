#include <aoclibs/common.h>
#include <aoclibs/error.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <string.h>

// === Split ===

static string_e split_from_len(const char *str, usize len) {
        Err err;
        String s = {0};

        err = heap_malloc(&s.heap, len + 1);
        if (err.code != 0) {
                return (string_e){ .err = err, .value = s };
        }
        memcpy(s.heap.ptr, str, len);
        ((char *)s.heap.ptr)[len] = '\0';
        s.length = len;
        return (string_e){ .err = ok(), .value = s };
}

static Err split_copy_buffer(String *dest, const char *src, usize len) {
        assert(!dest || !src);
        if (len + 1 > dest->heap.cap) {
                Err err = heap_realloc(&dest->heap, len + 1);
                if (err.code != ERR_SUCCESS) return err;
        }
        memcpy(dest->heap.ptr, src, len);
        ((char *)dest->heap.ptr)[len] = '\0';
        dest->length = len;
        return ok();
}

Err string_split(String *s, char delimiter, String *out_s1, String *out_s2) {
        assert(!s || !s->heap.ptr || !out_s1 || !out_s2);
        if (s->length == 0) return werr(-1, "String is empty");

        const char *ptr = (const char *)s->heap.ptr;
        usize i = 0;
        while (i < s->length && ptr[i] != delimiter)
                i++;

        Err err;
        string_e tmp_s1_err;
        string_e tmp_s2_err;
        String tmp_s1;
        String tmp_s2;

        if (i < s->length) {
                tmp_s1_err = split_from_len(ptr, i);
                tmp_s2_err = split_from_len(&ptr[i + 1], s->length - i - 1);
                tmp_s1 = tmp_s1_err.value;
                tmp_s2 = tmp_s2_err.value;
        } else {
                tmp_s1_err = _string_from(ptr, s->length);
                tmp_s2_err = a_string_from("");
                tmp_s1 = tmp_s1_err.value;
                tmp_s2 = tmp_s2_err.value;
        }

        err = split_copy_buffer(out_s1, tmp_s1.heap.ptr, tmp_s1.length);
        if (err.code != 0) {
                string_free(&tmp_s1);
                string_free(&tmp_s2);
                return err;
        }
        err = split_copy_buffer(out_s2, tmp_s2.heap.ptr, tmp_s2.length);
        if (err.code != 0) {
                string_free(&tmp_s1);
                string_free(&tmp_s2);
                return err;
        }

        string_free(&tmp_s1);
        string_free(&tmp_s2);
        return ok();
}

Err string_rsplit(String *s, char delimiter, String *out_s1, String *out_s2) {
        assert(!s || !s->heap.ptr || !out_s1 || !out_s2);
        if (s->length == 0) return werr(-1, "String is empty");

        const char *ptr = (const char *)s->heap.ptr;
        usize i = s->length;
        if (i > 0) i -= 1;

        while (i > 0 && ptr[i] != delimiter)
                i--;

        if (ptr[i] == delimiter) {
                Err err;
                string_e err_tmp_s1;
                string_e err_tmp_s2;
                String tmp_s1;
                String tmp_s2;

                err_tmp_s1 = split_from_len(ptr, i);

                if (err_tmp_s1.err.code != 0) {
                        if (err_tmp_s1.value.heap.ptr)
                                string_free(&err_tmp_s1.value);
                        return err_tmp_s1.err;
                }

                err_tmp_s2 = split_from_len(&ptr[i + 1], s->length - i - 1);

                if (err_tmp_s2.err.code != 0) {
                        if (err_tmp_s2.value.heap.ptr)
                                string_free(&err_tmp_s2.value);
                        return err_tmp_s2.err;
                }

                tmp_s1 = err_tmp_s1.value;
                tmp_s2 = err_tmp_s2.value;

                err = split_copy_buffer(out_s1, tmp_s1.heap.ptr,
                                         tmp_s1.length);
                if (err.code != 0) {
                        string_free(&tmp_s1);
                        string_free(&tmp_s2);
                        return err;
                }

                err = split_copy_buffer(out_s2, tmp_s1.heap.ptr,
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
                return werr(-1, "String s doesn't contain the delimiter");
        }
}
