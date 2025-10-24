#include "heap_alloc.h"
#include "heap_string.h"
#include <stdio.h>
#include <assert.h>

int main(void) {
        String *s1 = string_create();
        int err1 = string_copy(s1, "test");
        printf("%s\n", (char *)s1->heap.ptr);

        String *s = string_from("Hello");
        int locus = 0;
        if ((locus = string_find(s, 'o')) != -1) {
                printf("Found at index '%d'\n", locus);
        }

        String *s2 = string_from("Heklo");
        int err = sstrcmp(s, s2);
        printf("%d\n", err);

        string_append(s, ", world!");

        const char *mystr = "Hejlo, world!";
        err = strcmp(s, mystr);
        printf("%d\n", err);

        printf("s [memory: %zu] [length %zu]: %s\n", s->heap.mem, s->length,
               (char *)s->heap.ptr);
        printf("s2 [memory: %zu]: %s\n", s2->length, (char *)s2->heap.ptr);
        string_flush(s);
        printf("s [memory: %zu] [length %zu]: %s\n", s->heap.mem, s->length,
               (char *)s->heap.ptr);
        assert(strcmp(s, "Hello, world!") == 0);
        assert(strcmp(s2, "Heklo") == 0);

        string_free(s);
        string_free(s1);
        string_free(s2);

        String *s3 = string_from("hello");
        printf("%s [memory: %zu] [length: %zu]: ", "From 'hello'", s3->heap.mem,
               s3->length);
        printf("%s\n", (char *)s3->heap.ptr);
        string_push(s3, '!');
        printf("%s [memory: %zu] [length: %zu]: ", "Add bang '!'", s3->heap.mem,
               s3->length);
        printf("%s\n", (char *)s3->heap.ptr);
        string_pop(s3);
        printf("%s [memory: %zu] [length: %zu]: ", "Pop bang '!'", s3->heap.mem,
               s3->length);
        printf("%s\n", (char *)s3->heap.ptr);
        string_drop(s3, 1);
        printf("%s [memory: %zu] [length: %zu]: ",
               "Drop one character from index", s3->heap.mem, s3->length);
        printf("%s\n", (char *)s3->heap.ptr);
        string_flush(s3);
        printf("%s [memory: %zu] [length: %zu]: ", "Flush unused memory",
               s3->heap.mem, s3->length);
        printf("%s\n", (char *)s3->heap.ptr);

        string_free(s3);

        String *s4 = string_from("hello,world");
        String s4_1;
        String s4_2;
        string_split(s4, ',', &s4_1, &s4_2);

        printf("String 1: %s\n", (char *)s4_1.heap.ptr);
        printf("String 2: %s\n", (char *)s4_2.heap.ptr);
        printf("Original: %s\n", (char *)s4->heap.ptr);

        string_free(s4);

        return 0;
}
