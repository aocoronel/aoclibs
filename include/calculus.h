#ifndef AOC_CALCULUS_H_
#include "base.h"
#define AOC_CALCULUS_H_

// Turns 'x' into the next power of two
AOCDEF const size_t coerce_to_pow2(const size_t x);

#ifdef AOC_IMPLEMENTATION
#include "calculus.c"
#endif

#endif // AOC_CALCULUS_H_
