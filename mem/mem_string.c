#include "mem_error.h"
#include "mem_string.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t max_str_capacity = 126;

str aoc_str_create(size_t *cap) {
        if (cap == 0) {
                cap = &max_str_capacity;
        }
        char str[*cap];
        struct str string = {
                .length = 0,
                .capacity = *cap,
                .str = str,
        };
        return string;
}

MemError aoc_is_null_terminated(str *str) {
        if (str->str[str->length] == '\0')
                return MemSucess;
        else
                return MemNullNotFound;
}

MemError aoc_str_copy(str *string, const char *str, size_t str_len) {
        if (str == NULL || string == NULL) {
                return MemNoValue;
        }
        if (string->capacity < string->length + str_len) {
                return MemNoSpace;
        }
        memcpy(string->str, str, sizeof(char) * str_len);
        return MemSucess;
}

MemError aoc_str_str_copy(str *a, str *b) {
        if (a == NULL || b == NULL) {
                return MemNoValue;
        }
        if (a->capacity < b->length + a->length) {
                return MemNoSpace;
        }
        memcpy(a->str, b->str, sizeof(char) * b->length);
        return MemSucess;
}

// int aoc_str_cmp(const char *a, const char *b) {
//         return (a > b) - (a < b);
// }

size_t aoc_str_length(const char *str, size_t *cap) {
        size_t capacity;
        if (cap != NULL) {
                capacity = *cap;
        } else {
                capacity = max_str_capacity;
        }
        for (int i = 0; i < capacity; i++) {
                if (str[i] == '\0') return i;
        }
        return -1;
}

static void aoc_str_copy_raw(char *a, const char *b, size_t b_len) {
        for (int i = 0; i < b_len; i++) {
                a[i] = b[i];
        }
}

str aoc_str_from(char *a, size_t len, size_t *cap) {
        size_t capacity;
        if (cap != NULL) {
                capacity = *cap;
        } else {
#ifdef AOC_UNIFORM_MAX_CAPACITY
                capacity = max_str_capacity;
#else
                capacity = len;
#endif
        }
        if (len == 0) {
                return aoc_str_create(&capacity);
        }
        char b[capacity];
        if (a == NULL) {
                b[0] = '\0';
        } else {
                aoc_str_copy_raw(b, a, aoc_str_length(a, cap));
        }
        struct str string = {
                .length = len,
                .capacity = capacity,
                .str = b,
        };
        return string;
}

int main() {
        str mystr1 = aoc_str_create(NULL);
        printf("My Str (str): %s\nMy Str (capacity): %zu\n", mystr1.str,
               mystr1.capacity);

        str mystr2 = aoc_str_from("hi", aoc_str_length("hi", NULL), NULL);
        const char *str2 = "hello, there";
        printf("My Str (str): %s\nMy Str (capacity): %zu\n", mystr2.str,
               mystr2.capacity);

        MemError err = aoc_str_copy(&mystr2, str2, aoc_str_length(str2, NULL));
        aoc_explain_memerror(err, &mystr2);
        printf("My Str (str): %s\nMy Str (capacity): %zu\n", mystr2.str,
               mystr2.capacity);

        return 0;
}
