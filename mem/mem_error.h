#ifndef AOC_MEM_ERROR_H
#define AOC_MEM_ERROR_H

#include <stdio.h>
#include "mem_string.h"

typedef enum MemError { MemSucess, MemNoValue, MemNoSpace, MemNullNotFound } MemError;

void aoc_explain_memerror(int error, str *str);

#endif /* AOC_MEM_ERROR_H */
