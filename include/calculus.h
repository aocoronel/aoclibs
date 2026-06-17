#ifndef AOCLIBS_CALCULUS_H_
#define AOCLIBS_CALCULUS_H_

#include "base.h"

// Turns 'x' into the next power of two
AOCDEF const size_t coerce_to_pow2(const size_t x);

#ifdef AOCLIBS_IMPLEMENTATION
#include "calculus.c"
#endif

#endif // AOCLIBS_CALCULUS_H_
