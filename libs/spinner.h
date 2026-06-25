#ifndef AOCLIBS_SPINNER_H_
#define AOCLIBS_SPINNER_H_

#ifdef AOCLIBS_SPINNER

#include <stdint.h>

/*
 * Returns a dot spinner symbol.
 *
 * Using this function multiple times, gives the next spinner.
*/
const char *get_spinner(void);

#ifdef SPINNER_IMPLEMENTATION
AOCDEF const char *get_spinner(void) {
	static const char *SYMBOLS[] = { "⠁", "⠈", "⠐", "⠠", "⢀", "⡀", "⠄", "⠂" };
	static int_fast8_t counter = 0;
	const char *RESULT = SYMBOLS[counter];
	counter = (counter + 1) % (sizeof(SYMBOLS) / sizeof(SYMBOLS[0]));
	return RESULT;
}
#endif

#endif

#endif // AOCLIBS_SPINNER_H_
