#ifndef AOC_MEM_STRING_H
#define AOC_MEM_STRING_H

#include <stddef.h>

/* Fallback value, in case a capacity is ommited in functions */
extern size_t max_str_capacity;

typedef struct str {
        size_t length;
        size_t capacity;
        char *str;
} str;

#endif /* AOC_MEM_STRING_H */
