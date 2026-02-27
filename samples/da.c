#define AOCLIBS_IMPLEMENTATION
#include "base.h"
#include <stdio.h>
#include <string.h>
#define AOCLIBS_STRIP_PREFIX
#include <stdint.h>
#include <stdlib.h>
#include "cstr.h"
#include "rc.h"
#include "da.h"

typedef struct {
        size_t cap;
        size_t len;
        int *data;
} Da_Int;

typedef struct {
        size_t cap;
        size_t len;
        char *data;
} Da_Char;

int compare_string(const void *a, const void *b) {
        char ca = tolower(*(const char *)a);
        char cb = tolower(*(const char *)b);
        if (ca != cb) return ca - cb;
        return *(const char *)a - *(const char *)b;
}

int main(int argc, char *argv[]) {
        Da_Char my_da = { 0 };

        // If you know before hand you need many allocations:
        // preallocate space (500 * sizeof(*my_da.data))
        da_reserve(&my_da, 500);

        // my_da.data can be NULL here, it's going to allocate a new buffer
        da_copy(&my_da, "ABC", 3);
        da_copy(&my_da, "abc", 3);

        Da_Char my_da2 = { 0 };
        da_clone(&my_da2, &my_da);
        my_da2.data[0] = 'b';

        printf("before sort: %s\n", my_da2.data);
        da_sort(&my_da2, compare_string);
        printf("after sort:%s\n", my_da2.data);

        printf("0: %c\n1: %c\n", my_da2.data[0], my_da2.data[1]);
        da_swap(char, &my_da2, 0, 1);
        printf("0: %c\n1: %c\n", my_da2.data[0], my_da2.data[1]);
        printf("%c (len: %zu)\n", da_last(&my_da2), my_da2.len);

        // In this case, the len is reduced by 1, but this still prints 6
        // Somehow myda2.len is calculated before the pop operation
        printf("%c (len: %zu)\n", da_pop(&my_da2), my_da2.len);
        // To confirm it's indeed 5:
        printf("%zu\n", my_da2.len);

        da_free(&my_da);

        int buff[256];
        Da_Int myint = { 0 };
        myint.data = buff;
        myint.cap = 256;

        // myint.data can NOT be NULL here. If you don't set the NDEBUG flag
        // this will assert myint.data != NULL
        int int2 = 1;
        if aoc_das_append (&myint, &int2, 1) else NULL;
        printf("%d (%zu:%zu)\n", myint.data[0], myint.len, myint.cap);

        if aoc_das_insert (&myint, int2) else NULL;
        printf("%d (%zu:%zu)\n", myint.data[0], myint.len, myint.cap);

        int d[] = { 1, 0, 3 };
        if aoc_das_append (&myint, d, array_len(d)) else NULL;

        if aoc_das_append_null (&myint) else NULL;

        foreach (&myint, i) {
                printf("%d\n", myint.data[i]);
        }

        da_erase(&myint);

        char mychar_buff[256];
        rc mychar = rcs_bnew(mychar_buff, 0);
        const char* lti = "hello, world! dog dog dog";
        assert(strlen(lti) == lcstrlen("hello, world! dog dog dog"));
        if lrcs_cat (&mychar, "hello, world! dog dog dog") else printf("failed");
        if lrcs_cat (&mychar, "hello, world! dog dog dog") else printf("failed");
        if lrcs_append (&mychar, "hello, world! dog dog dog") else printf("failed");
        if lrcs_append (&mychar, "hello, world! dog dog dog") else printf("failed");

        if (aoc_da_is_null(&mychar)) printf("mychar is null!");
        printf("%s (%zu:%zu)\n", mychar.data, mychar.len, mychar.cap);

        rc mychar2 = rc_new(256);

        aoc_rc_cat(&mychar2, "hello, world! dog dog dog", 25);
        aoc_rc_cat(&mychar2, "hello, world! dog dog dog", 25);
        printf("string: %s (%zu)\n", mychar2.data, mychar2.len);

        return 0;
}
