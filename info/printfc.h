#pragma once

#define COLOR_RED "\x1b[91m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_GREEN "\x1b[38;5;47m"
#define COLOR_BLUE "\x1b[38;5;75m"
#define COLOR_RESET "\x1b[0m"

/*
 * Holds printfc options
*/
typedef enum { FATAL, ERROR, WARN, INFO, DEBUG } LogLevel;

/*
 * Write formatted output to stderr or stdout
 *
 * FATAL, ERROR => stderr
 *
 * INFO, DEBUG, WARN => stdout
 *
 * printfc(FATAL, "Something went very wrong: %s", "disk not found");
*/
int printfc(LogLevel level, const char *restrict fmt, ...);
