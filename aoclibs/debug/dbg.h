#ifndef AOCLIBS_DEBUG_DBG_H
#define AOCLIBS_DEBUG_DBG_H

#ifdef NDEBUG
#define dbg(...) ((void)0)
#else
#define dbg(...) fprintf(stderr, __VA_ARGS__)
#endif /* NDEBUG */

#endif
