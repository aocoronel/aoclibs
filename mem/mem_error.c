#include <stdbool.h>
#include <stdio.h>
#include "mem_error.h"
#include "mem_string.h"

#ifdef AOC_DEBUG
static void aoc_explain_str(bool explain, str *str) {
        printf("%s (len = %zu) (cap = %zu) :: ", str->str, str->length,
               str->capacity);
}
#endif

void aoc_explain_memerror(int error, str *str) {
        bool explain_str;
        if (str == NULL) {
                explain_str = false;
        } else {
                explain_str = true;
                #ifdef AOC_DEBUG
                aoc_explain_str(explain_str, str);
                #endif
        }

        switch (error) {
        case MemNoValue:
                printf("No value error\n");
                break;
        case MemNoSpace:
                printf("No space error\n");
                break;
        case MemNullNotFound:
                printf("Null not found error\n");
                break;
        default:
                printf("Unknown error\n");
        }
}
