#ifndef AOCLIBS_SPINNER_H_
#define AOCLIBS_SPINNER_H_

#ifdef AOCLIBS_SPINNER

#include <stdint.h>

/*
 * Returns a dot spinner symbol.
 *
 * Using this function multiple times, gives the next spinner.
*/
const char *aoc_get_spinner(void);

#ifdef AOCLIBS_STRIP_PREFIX
#define get_spinner aoc_get_spinner
#endif

#define AOC_SPINNER_NUM_SYMBOLS (sizeof(SYMBOLS) / sizeof(SYMBOLS[0]))

#ifdef SPINNER_IMPLEMENTATION
const char *aoc_get_spinner(void) {
    static const char *SYMBOLS[] = { "⠁", "⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂" };
    static int_fast8_t counter = 0;
    const char *RESULT = SYMBOLS[counter];
    counter = (counter + 1) % AOC_SPINNER_NUM_SYMBOLS;
    return RESULT;
}
#endif

#endif

#endif // AOCLIBS_SPINNER_H_
