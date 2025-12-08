#ifndef AOCLIBS_IO_PRINTFC_H
#define AOCLIBS_IO_PRINTFC_H

#define PRINTFC_COLOR_RED "\x1b[91m"
#define PRINTFC_COLOR_YELLOW "\x1b[33m"
#define PRINTFC_COLOR_GREEN "\x1b[38;5;47m"
#define PRINTFC_COLOR_BLUE "\x1b[38;5;75m"
#define PRINTFC_COLOR_RESET "\x1b[0m"

#ifdef NDEBUG
#define PDEBUG(...) ((void)0)
#else
#ifndef PDEBUG
#define PDEBUG(...) printfc(DEBUG, __VA_ARGS__)
#endif // PDEBUG
#endif // NDEBUG

/*
 * Holds printfc options
*/
typedef enum { FATAL, ERROR, WARN, INFO, DEBUG } LogLevel;

/*
 * Print colored and formatted messages to stderr, based on LogLevel
 *
 * If the LogLevel is FATAL, printfc also flushes the stdout
*/
void printfc(LogLevel level, const char *fmt, ...);

#endif
