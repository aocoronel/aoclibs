#ifndef AOC_LIBC_H_
#define AOC_LIBC_H_

// These are the most used LibC libraries, and all libraries from aoclibs are assumed to never
// include these separatedly. You can take this into advantage to refactor them away.

#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>

#ifdef __cplusplus
#include <atomic>
#else
#include <stdatomic.h>
#endif

#endif
