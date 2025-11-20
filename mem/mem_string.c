#include "mem_error.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define str_from(str, cap) aoc_str_from((str), aoc_str_length(str, cap), (cap))

typedef struct str {
        size_t length;
        size_t capacity;
        char *str;
} str;

str aoc_str_create(size_t cap) {
        if (cap == 0) {
                str string;
                return string;
        }
        char str[cap];
        struct str string = {
                .length = 0,
                .capacity = cap,
                .str = str,
        };
        return string;
}

int8_t aoc_is_null_terminated(str *str) {
        if (str->str[str->length] == '\0')
                return 0;
        else
                return 1;
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

int aoc_str_cmp(const char a, const char b) {
        return (a > b) - (a < b);
}

size_t aoc_str_length(const char *str, size_t cap) {
        for (int i = 0; i < cap; i++) {
                if (aoc_str_cmp(str[i], '\0') == 0) return i;
        }
        return -1;
}

str aoc_str_from(char *str, size_t len, size_t cap) {
        if (cap == 0) {
                cap = len;
        }
        if (len == 0) {
                return aoc_str_create(cap);
        }
        if (str == NULL) {
                str = "";
        }
        struct str string = {
                .length = len,
                .capacity = cap,
                .str = str,
        };
        return string;
}

int main() {
        // str mystr = aoc_str_create(10);
        // printf("My Str (str): %s\nMy Str (capacity): %zu", mystr.str,
        //        mystr.capacity);
        str mystr = str_from("hi", 10);
        const char *str2 = "hello, there";
        printf("My Str (str): %s\nMy Str (capacity): %zu\n", mystr.str,
               mystr.capacity);
        aoc_str_copy(&mystr, str2, aoc_str_length(str2, 40));
        printf("My Str (str): %s\nMy Str (capacity): %zu\n", mystr.str,
               mystr.capacity);
        return 0;
}
