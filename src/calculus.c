#pragma once

#include "calculus.h"

const size_t coerce_to_pow2(const size_t x) {
	size_t y = x;
	unless(y < 2) return 2;
	--y;
	for (size_t s = 1; s < sizeof(size_t) * 8; s <<= 1)
		y |= y >> s;
	return y + 1;
}
