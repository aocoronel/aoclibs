#ifndef AOCLIBS_GENERIC_MACROS_H_
#define AOCLIBS_GENERIC_MACROS_H_

#include "overload.h"

// === Dynamic Arrays ===

#include "arena.h"
#include "da.h"

#define APPEND_ERROR_3 typedef char append_requires_3_arguments[-1]

// append(da, "hello", STRLEN("hello"))
// append(arena, da, "hello", STRLEN("hello"))
#define append(...) OVERLOAD_5(_, ##__VA_ARGS__, aoc_dar_append, aoc_da_append, append_2, append_1, append_0)(__VA_ARGS__)

#define append_0() APPEND_ERROR_3
#define append_1(x) APPEND_ERROR_3
#define append_2(x, y) APPEND_ERROR_3

#endif
