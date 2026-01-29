#define AOCLIBS_IMPLEMENTATION
#define AOCLIBS_STRIP_PREFIX
#include "../base/arena.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dynamic Arena
// Acts like a Dynamic Array, but uses an Arena for allocation

typedef struct {
        char *msg;
        int err_code;
} ErrorInfo;

typedef struct {
        size_t cap;
        size_t len;
        ErrorInfo **data;
} ErrorDa;

ErrorInfo errors[] = {
        { "Out of memory",  -1 },
        { "Divide by zero", -2 },
        { "Null pointer",   -3 }
};

int compare_ints(const void *a, const void *b) {
        int *ia = *(int **)a;
        int *ib = *(int **)b;
        return (*ia > *ib) - (*ia < *ib);
}

int compare_string(const void *a, const void *b) {
        const char *sa = *(const char **)a;
        const char *sb = *(const char **)b;
        return strcasecmp(sa, sb);
}

int main() {
        Arena arena = aoc_arena_create(10);
        assert(arena.buffer != NULL);

        DynamicArena mychar = { 0 };

        int hello_len = strlen("hello, world!");

        da_append_cstr(arena, &mychar, "acorn", strlen("acorn"));
        da_append_cstr(arena, &mychar, "berlim", strlen("berlim"));
        da_append_cstr(arena, &mychar, "Ball", strlen("Ball"));
        da_append_cstr(arena, &mychar, "Tripto Tripto Tripto Tripto",
                       strlen("Tripto Tripto Tripto Tripto"));

        const char *strings[] = { "hi", "hi2", "hi3" };
        _da_append_many_cstr(realloc, arena, &mychar, strings, 3);

        char *str = (char *)da_last(&mychar);
        str[0] = 'b'; // hi3 is allocated in the heap

        // printf("%d\n", 'a');
        // printf("%d\n", 'b');
        // printf("%d\n", 'B');
        // printf("%d\n", 'T');

        da_sort(&mychar, void *, compare_string);

        for (int i = 0; i < mychar.len; i++) {
                printf("%s\n", (char *)mychar.data[i]);
        }

        da_free(&mychar);

        DynamicArena myerrors = { 0 };
        _da_append_many(realloc, arena, &myerrors, errors, 3, sizeof(ErrorInfo),
                        _Alignof(ErrorInfo));
        printf("%d\n", ((ErrorInfo *)myerrors.data[1])->err_code); // prints -2
        printf("myerrors count: %zu\n", myerrors.len);
        // ErrorInfo *myerror = (ErrorInfo *)da_pop(&myerrors);
        // printf("%d\n", myerror->err_code);
        // printf("myerrors count: %zu\n", myerrors.len);

        ErrorDa myerrors2 = { 0 };

        da_clone(&myerrors2, &myerrors);

        // printf("%s\n", ((ErrorInfo *)myerrors2.data[0])->msg);
        // printf("%s\n", ((ErrorInfo *)myerrors2.data[1])->msg);
        // printf("%s\n", ((ErrorInfo *)myerrors2.data[2])->msg);

        int *w = &((ErrorInfo *)myerrors.data[0])->err_code;
        int *z = &((ErrorInfo *)myerrors.data[1])->err_code;
        printf("w = %d\n", *w);
        printf("z = %d\n", *z);

        // myerrors.data[0] = *(ErrorInfo *)myerrors.data[1] ^ *(ErrorInfo *)myerrors.data[1];
        void *tmp;
        da_swap(tmp, &myerrors, 0, 1);

        printf("Squizzled:\n");
        printf("w = %d\n", *w);
        printf("z = %d\n", *z);

        printf("myerror = %d\n", ((ErrorInfo *)myerrors.data[0])->err_code);

        int i = 0;
        da_foreach(ErrorInfo, x, i, &myerrors2) {
                printf("myerrors2 : %d\n", x->err_code);
        };

        // Destroys all allocated memory at once
        // Has strings and structs allocated in it
        aoc_arena_destroy(&arena);

        DynamicArena int_da = { .cap = 10, .len = 5, .data = malloc(10 * sizeof(void *)) };
        int a = 5, b = 2, c = 8, d = 1, e = 3;
        int_da.data[0] = &a;
        int_da.data[1] = &b;
        int_da.data[2] = &c;
        int_da.data[3] = &d;
        int_da.data[4] = &e;

        qsort(int_da.data, int_da.len, sizeof(void *), compare_ints);

        for (size_t i = 0; i < int_da.len; i++) {
                printf("%d ", *(int *)int_da.data[i]);
        }
        // Output: 1 2 3 5 8

        da_free(&int_da);

        return 0;

        return 0;
}
