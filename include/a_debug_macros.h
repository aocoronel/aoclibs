#ifndef AOCLIBS_DEBUG_MACROS_H
#define AOCLIBS_DEBUG_MACROS_H

#ifdef NDEBUG

#define PDEBUG(...) ((void)0)
#else

// printfc takes precedence
#ifdef AOCLIBS_IO_PRINTFC_H
#define PDEBUG(...) printfc(DEBUG, __VA_ARGS__)
#else
#define PDEBUG(...) fprintf(stderr, __VA_ARGS__)
#endif /* AOCLIBS_IO_PRINTFC_H */

#endif /* NDEBUG */

#endif
