#pragma once

#include "calculus.h"

size_t coerce_to_pow2(size_t x) {
	if (x < 2) return 2;
	--x;
	for (size_t s = 1; s < sizeof(size_t) * 8; s <<= 1)
		x |= x >> s;
	return x + 1;
}
