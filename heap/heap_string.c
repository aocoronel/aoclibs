#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "heap_alloc.h"
#include "heap_string.h"

String *string_create(void) {
        String *s = malloc(sizeof(String));
        if (!s) return NULL;
        heap_init(&s->heap);
        if (!heap_alloc(&s->heap, 16)) {
                free(s);
                return NULL;
        }
        s->length = 0;
        ((char *)s->heap.ptr)[0] = '\0';
        return s;
}

String *string_from(const char *str) {
        String *s = malloc(sizeof(String));
        if (!s) return NULL;
        size_t len = strlen(str);
        heap_init(&s->heap);
        if (!heap_alloc(&s->heap, len + 1)) {
                free(s);
                return NULL;
        }
        memcpy(s->heap.ptr, str, len + 1);
        s->length = len;
        return s;
}

void string_free(String *s) {
        if (s == NULL) return;
        heap_free(&s->heap);
        free(s);
}

static int string_reserve(String *s, size_t capacity) {
        if (s == NULL) {
                return -1;
        }
        if (capacity <= s->heap.mem) return 0;
        if (!resize_alloc(&s->heap, capacity)) return -1;
        return 0;
}

int string_append(String *s, const char *str) {
        if (s == NULL) {
                return -1;
        }
        size_t add_len = strlen(str);
        size_t needed = s->length + add_len + 1;
        if (needed > s->heap.mem) {
                if (string_reserve(s, needed * 2) != 0) return -1;
        }
        memcpy((char *)s->heap.ptr + s->length, str, add_len);
        ((char *)s->heap.ptr)[s->length + add_len] = '\0';
        s->length += add_len;
        return 0;
}

int string_string_append(String *s1, const String *s2) {
        if (s1 == NULL || s2 == NULL) {
                return -1;
        }

        size_t add_len = s2->length;
        size_t needed = s1->length + add_len + 1;

        if (needed > s1->heap.mem) {
                if (string_reserve(s1, needed * 2) != 0) return -1;
        }

        memcpy((char *)s1->heap.ptr + s1->length, s2->heap.ptr, add_len);
        ((char *)s1->heap.ptr)[s1->length + add_len] = '\0';
        s1->length += add_len;

        return 0;
}

int string_copy(String *s, const char *str) {
        if (s == NULL) {
                return -1;
        }
        size_t add_len = strlen(str);
        size_t needed = add_len + 1;
        if (needed > s->heap.mem) {
                if (string_reserve(s, needed * 2) != 0) return -1;
        }
        memcpy((char *)s->heap.ptr, str, add_len + 1);
        s->length += add_len;
        return 0;
}

int string_string_copy(String *s1, const String *s2) {
        if (s1 == NULL || s2 == NULL) {
                return -1;
        }

        size_t needed = s2->length + 1;

        if (needed > s1->heap.mem) {
                if (string_reserve(s1, needed * 2) != 0) return -1;
        }

        memcpy((char *)s1->heap.ptr, s2->heap.ptr, needed);

        s1->length = s2->length;

        return 0;
}

int string_find(String *s, const char c) {
        if (s == NULL || s->heap.ptr == NULL || s->length == 0) {
                return -1;
        }

        const char *ptr = (const char *)s->heap.ptr;
        for (size_t i = 0; i < s->length; i++) {
                if (ptr[i] == c) {
                        return (int)i;
                }
        }
        return -1;
}

int string_reverse_find(String *s, const char c) {
        if (s == NULL || s->heap.ptr == NULL || s->length == 0) {
                return -1;
        }
        const char *ptr = (const char *)s->heap.ptr;
        for (size_t i = s->length; i > 0; i--) {
                if (ptr[i - 1] == c) {
                        return (int)(i - 1);
                }
        }
        return -1;
}

int string_compare(String *s, const char *str) {
        if (s == NULL || str == NULL) {
                return -1;
        }
        size_t len = strlen(str);
        if (s->length != len) return (int)s->length - len;
        return memcmp(s->heap.ptr, str, s->length);
}

int string_string_compare(const String *s1, const String *s2) {
        if (s1 == NULL || s2 == NULL) {
                return -1;
        }
        if (s1->length != s2->length) return (int)s1->length - (int)s2->length;
        return memcmp(s1->heap.ptr, s2->heap.ptr, s1->length);
}

void string_trim_trailing(String *s) {
        if (!s || !s->heap.ptr || s->length == 0) return;

        char *ptr = (char *)s->heap.ptr;
        size_t i = s->length;
        while (i > 0 && isspace((unsigned char)ptr[i - 1])) {
                i--;
        }
        s->length = i;
        ptr[i] = '\0';
}

// === Flush ===

static int string_shrink_to_fit(String *s) {
        size_t needed = s->length + 1;
        if (needed < s->heap.mem) {
                if (!resize_alloc(&s->heap, needed)) {
                        return -1;
                }
        }
        return 0;
}

int string_flush(String *s) {
        if (!s || !s->heap.ptr) return -1;

        string_trim_trailing(s);
        return string_shrink_to_fit(s);
}

// END === Flush ===

int string_push(String *s, char c) {
        if (!s || !s->heap.ptr) return -1;

        size_t needed = s->length + 2;
        if (needed > s->heap.mem) {
                if (string_reserve(s, needed) != 0) return -1;
        }

        ((char *)s->heap.ptr)[s->length] = c;
        ((char *)s->heap.ptr)[s->length + 1] = '\0';
        s->length++;
        return 0;
}

int string_pop(String *s) {
        if (!s || !s->heap.ptr || s->length == 0) return -1;

        s->length--;
        ((char *)s->heap.ptr)[s->length] = '\0';
        return 0;
}

int string_drop(String *s, size_t index) {
        if (!s || !s->heap.ptr || index >= s->length) return -1;

        char *ptr = (char *)s->heap.ptr;
        memmove(&ptr[index], &ptr[index + 1], s->length - index);
        s->length--;
        ptr[s->length] = '\0';
        return 0;
}

// === Split ===

static String *string_from_len(const char *str, size_t len) {
        String *s = malloc(sizeof(String));
        if (!s) return NULL;
        heap_init(&s->heap);
        if (!heap_alloc(&s->heap, len + 1)) {
                free(s);
                return NULL;
        }
        memcpy(s->heap.ptr, str, len);
        ((char *)s->heap.ptr)[len] = '\0';
        s->length = len;
        return s;
}

static int string_copy_buffer(String *dest, const char *src, size_t len) {
        if (!dest || !src) return -1;
        if (len + 1 > dest->heap.mem) {
                if (!resize_alloc(&dest->heap, len + 1)) return -1;
        }
        memcpy(dest->heap.ptr, src, len);
        ((char *)dest->heap.ptr)[len] = '\0';
        dest->length = len;
        return 0;
}

int string_split(String *s, char delimiter, String *out_s1, String *out_s2) {
        if (!s || !s->heap.ptr || s->length == 0 || !out_s1 || !out_s2)
                return -1;

        const char *ptr = (const char *)s->heap.ptr;
        size_t i = 0;
        while (i < s->length && ptr[i] != delimiter)
                i++;

        String *tmp_s1 = NULL;
        String *tmp_s2 = NULL;

        if (i < s->length) {
                tmp_s1 = string_from_len(ptr, i);
                tmp_s2 = string_from_len(&ptr[i + 1], s->length - i - 1);
        } else {
                tmp_s1 = string_from(ptr);
                tmp_s2 = string_from("");
        }

        if (!tmp_s1 || !tmp_s2) {
                if (tmp_s1) string_free(tmp_s1);
                if (tmp_s2) string_free(tmp_s2);
                return -1;
        }

        if (string_copy_buffer(out_s1, tmp_s1->heap.ptr, tmp_s1->length) != 0) {
                string_free(tmp_s1);
                string_free(tmp_s2);
                return -1;
        }
        if (string_copy_buffer(out_s2, tmp_s2->heap.ptr, tmp_s2->length) != 0) {
                string_free(tmp_s1);
                string_free(tmp_s2);
                return -1;
        }

        string_free(tmp_s1);
        string_free(tmp_s2);
        return 0;
}

// END === Split ===

void string_clear(String *s) {
        if (!s || !s->heap.ptr) return;
        ((char *)s->heap.ptr)[0] = '\0';
        s->length = 0;
}

void string_garbage(void *ptr, size_t len) {
        if (!ptr) return;
        volatile unsigned char *p = ptr;
        while (len--)
                *p++ = 0;
}
