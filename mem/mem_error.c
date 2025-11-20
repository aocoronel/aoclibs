#include <stdint.h>
#include <stdio.h>

typedef enum MemError { MemNoValue = 1, MemNoSpace, MemNullNotFound } MemError;

void aoc_explain_memerror(int error) {
        switch (error) {
        case MemNoValue:
                printf("No value error ");
                break;
        case MemNoSpace:
                printf("No space error ");
                break;
        case MemNullNotFound:
                printf("Null not found error ");
                break;
        default:
                printf("Unknown error ");
        }
}
