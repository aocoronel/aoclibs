#ifndef AOCLIBS_DEBUG_DBG_H
#define AOCLIBS_DEBUG_DBG_H

#ifdef NDEBUG
#define DBG(...) ((void)0)
#else
#define DBG(...) fprintf(stderr, __VA_ARGS__)
#endif /* NDEBUG */

#endif
