#ifndef AOCLIBS_IPRINT_H_
#define AOCLIBS_IPRINT_H_

#include "base/attributes.h"

/*
 * Prints indented message
 *
 * If the size of the terminal cannot be obtained, fallback to 80 columns
*/
AOCLIBS_PREFIX void aoc_iprint(const char *msg, int indent);

#endif // AOCLIBS_IPRINT_H_
