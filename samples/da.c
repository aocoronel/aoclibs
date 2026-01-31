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

ErrorInfo errors[] = {
        { "Out of memory",  -1 },
        { "Divide by zero", -2 },
        { "Null pointer",   -3 }
};

int main() {
        Arena arena = aoc_arena_create(10);
        assert(arena.buffer != NULL);

        DynamicArena mychar = { 0 };
        mychar.arena = &arena;

        int hello_len = strlen("hello, world!");

        dar_append_cstr(&mychar, "acorn", strlen("acorn"));
        // or
        dar_append_cstrl(&mychar, "acorn");

        printf("last: %s\n", (char *)dar_last(&mychar));

        dar_append_cstr(&mychar, "berlim", strlen("berlim"));

        printf("last: %s\n", (char *)dar_last(&mychar));

        dar_append_cstr(&mychar, "Ball", strlen("Ball"));

        printf("last: %s\n", (char *)dar_last(&mychar));

        dar_append_cstr(&mychar, "Tripto Tripto Tripto Tripto",
                        strlen("Tripto Tripto Tripto Tripto"));

        printf("last: %s\n", (char *)dar_last(&mychar));

        const char *strings[] = { "hi", "hi2", "hi3" };
        dar_append_many_cstr(&mychar, strings, 3);

        char *str = (char *)dar_last(&mychar);
        str[0] = 'b'; // hi3 is allocated in the heap

        for (int i = 0; i < mychar.len; i++) {
                printf("%s\n", (char *)dar_get(&mychar, i));
        }

        DynamicArena myerrors = { 0 };
        myerrors.arena = &arena;
        dar_append_many(&myerrors, errors, 3, sizeof(ErrorInfo), _Alignof(ErrorInfo));
        printf("%d\n", ((ErrorInfo *)dar_get(&myerrors, 1))->err_code); // prints -2
        printf("myerrors count: %zu\n", myerrors.len);
        ErrorInfo *myerror = (ErrorInfo *)dar_pop(&myerrors);
        printf("Value returned from pop: %d\n", myerror->err_code);
        printf("Current myerror: last value is %d and index is %zu\n",
               ((ErrorInfo *)dar_last(&myerrors))->err_code, myerrors.len);

        foreach(&myerrors, i) {
                ErrorInfo *x = (ErrorInfo *)dar_get(&myerrors, i);
                printf("Error Code (Index %zu) : %d\n", i, x->err_code);
        };

        printf("Before squizzle\n");
        printf("w = %d\n", ((ErrorInfo *)dar_get(&myerrors, 0))->err_code); // -1
        printf("z = %d\n", ((ErrorInfo *)dar_get(&myerrors, 1))->err_code); // -2

        dar_swap(&myerrors, 0, 1);

        printf("Squizzled:\n");
        printf("w = %d\n", ((ErrorInfo *)dar_get(&myerrors, 0))->err_code); // -2
        printf("z = %d\n", ((ErrorInfo *)dar_get(&myerrors, 1))->err_code); // -1

        printf("myerror = %d\n", ((ErrorInfo *)dar_get(&myerrors, 0))->err_code);

        DynamicArena int_da = { .arena = &arena };
        int a = 5, b = 2, c = 8, d = 1, e = 3;

        dar_append(&int_da, &a, sizeof(int));
        dar_append(&int_da, &b, sizeof(int));
        dar_append(&int_da, &c, sizeof(int));
        dar_append(&int_da, &d, sizeof(int));
        dar_append(&int_da, &e, sizeof(int));

        int *i = (int *)dar_get(&int_da, 0);
        printf("%d\n", *i);

        dar_set(int *, &int_da, 0, b);
        printf("%d\n", *i);

        for (size_t i = 0; i < int_da.len; i++) {
                printf("%d ", *(int *)dar_get(&int_da, i));
        }
        // Output: 2 2 8 1 3

        dar_free(&int_da);

        // Notice how just a single arena was used.
        // It was used to allocate strings, structs and ints.
        //
        // Arenas are particularly not useful to store strings that
        // are known to need to grow, since this implementation can
        // waste (not leak) a little bit of memory for each reallocation.
        //
        // Whenever the string needs to grow, a new part of the arena is
        // reserved and all the previous data is copied to the new location.
        // The old one is still valid, and contains the same data.
        //
        // I personally don't know if it's worth to keep it as is, or to
        // try to make the Arena a little bit smarter and slower to save some
        // memory.
        //
        // Destroys all allocated memory at once with a single free call.
        aoc_arena_destroy(&arena);

        return 0;
}
