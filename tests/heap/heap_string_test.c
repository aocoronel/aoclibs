#include <heap_alloc.h>
#include <heap_string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

String default_string(void) {
        stringerr s1_err = string_create(0);
        String s1 = s1_err.value;
        str_cpy(&s1, "world!");
        str_cat(&s1, " Hello,");
        return s1;
}

int main(void) {
        stringerr s1 = string_create(0);
        int err1 = string_copy(&s1.value, "test");
        printf("%s\n", (char *)s1.value.heap.ptr);

        stringerr s = string_from("Hello");
        int locus = 0;
        if ((locus = string_find(&s.value, 'o')) != -1) {
                printf("Found at index '%d'\n", locus);
        }

        stringerr s2 = string_from("Heklo");
        int err = sstr_cmp(&s.value, &s2.value);
        printf("%d\n", err);

        string_append(&s.value, ", world!");

        const char *mystr = "Hejlo, world!";
        err = str_cmp(&s.value, mystr);
        printf("%d\n", err);

        printf("s [memory: %zu] [length %zu]: %s\n", s.value.heap.mem, s.value.length,
               (char *)s.value.heap.ptr);
        printf("s2 [memory: %zu]: %s\n", s2.value.length, (char *)s2.value.heap.ptr);
        string_flush(&s.value);
        printf("s [memory: %zu] [length %zu]: %s\n", s.value.heap.mem, s.value.length,
               (char *)s.value.heap.ptr);
        assert(str_cmp(&s.value, "Hello, world!") == 0);
        assert(str_cmp(&s2.value, "Heklo") == 0);

        string_free(&s.value);
        string_free(&s1.value);
        string_free(&s2.value);

        stringerr s3 = string_from("hello");
        printf("%s [memory: %zu] [length: %zu]: ", "From 'hello'",
               s3.value.heap.mem, s3.value.length);
        printf("%s\n", (char *)s3.value.heap.ptr);
        string_push(&s3.value, '!');
        printf("%s [memory: %zu] [length: %zu]: ", "Add bang '!'",
               s3.value.heap.mem, s3.value.length);
        printf("%s\n", (char *)s3.value.heap.ptr);
        string_pop(&s3.value);
        printf("%s [memory: %zu] [length: %zu]: ", "Pop bang '!'",
               s3.value.heap.mem, s3.value.length);
        printf("%s\n", (char *)s3.value.heap.ptr);
        string_drop(&s3.value, 1);
        printf("%s [memory: %zu] [length: %zu]: ",
               "Drop one character from index", s3.value.heap.mem,
               s3.value.length);
        printf("%s\n", (char *)s3.value.heap.ptr);
        string_flush(&s3.value);
        printf("%s [memory: %zu] [length: %zu]: ", "Flush unused memory",
               s3.value.heap.mem, s3.value.length);
        printf("%s\n", (char *)s3.value.heap.ptr);

        string_free(&s3.value);

        stringerr s4_err = string_from("hello,my,world");
        String s4 = s4_err.value;
        String s4_1, s4_2;
        heap_init(&s4_1.heap);
        heap_init(&s4_2.heap);
        if (string_split(&s4, ',', &s4_1, &s4_2) != 0) {
                fprintf(stderr, "splitting failed\n");
                return EXIT_FAILURE;
        }
        printf("String 1: %s\n", (char *)s4_1.heap.ptr);
        printf("String 2: %s\n", (char *)s4_2.heap.ptr);
        printf("Original: %s\n", (char *)&s4.heap.ptr);

        heap_free(&s4_1.heap);
        heap_free(&s4_2.heap);
        string_free(&s4);

        stringerr s5_err = string_from("hello,my,world");
        String s5 = s5_err.value;
        String s5_1, s5_2;
        heap_init(&s5_1.heap);
        heap_init(&s5_2.heap);
        if (string_rsplit(&s5, ',', &s5_1, &s5_2) != 0) {
                fprintf(stderr, "splitting failed\n");
                return EXIT_FAILURE;
        }
        printf("String 1: %s\n", (char *)s5_1.heap.ptr);
        printf("String 2: %s\n", (char *)s5_2.heap.ptr);
        printf("Original: %s\n", (char *)&s5.heap.ptr);

        heap_free(&s5_1.heap);
        heap_free(&s5_2.heap);
        string_free(&s5);

        return 0;
}
