#ifndef AOCLIBS_DEBUG_H
#define AOCLIBS_DEBUG_H

#ifdef NDEBUG
#define DEBUG(...)
#else
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#endif

#endif
