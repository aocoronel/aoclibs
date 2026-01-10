#ifndef AOCLIBS_SPINNER_H_
#define AOCLIBS_SPINNER_H_

/*
 * Returns a dot spinner symbol.
 *
 * Using this function multiple times, gives the next spinner.
*/
const char *aoc_get_spinner(void);

#ifdef AOCLIBS_STRIP_PREFIX
#define get_spinner() aoc_get_spinner
#endif

#endif // AOCLIBS_SPINNER_H_
