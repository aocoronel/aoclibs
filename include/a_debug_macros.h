#ifndef AOCLIBS_DEBUG_MACROS_H
#define AOCLIBS_DEBUG_MACROS_H

#ifdef NDEBUG
#define DEBUG(...)
#else
#include <stdio.h>
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#endif

#endif
