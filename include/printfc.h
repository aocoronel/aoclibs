#ifndef AOC_PRINTFC_H_
#define AOC_PRINTFC_H_

#include "base.h"
#include "colors.h"

// printfc :: print formatted color

/*
 * Print colored and formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
 */

/*
 * printfc_trace and printfc_debug are special. They will only run if NDEBUG
 * define is not defined.
 */

#ifdef NDEBUG
#define printfc_debug(...) ((void)0)
#define printfc_trace(...) ((void)0)
#else // NDEBUG
#define printfc_debug(...) printfc(COLOR_CYAN, "debug", __VA_ARGS__)
#define printfc_trace(...) printfc(COLOR_MAGENTA, "trace", __VA_ARGS__)
#endif // NDEBUG

#define printfc(color, level, ...) \
	(fprintf(stderr, "%s%s:%s ", color, level, COLOR_RESET), fprintf(stderr, __VA_ARGS__))
#define printfc_fatal(...) printfc(COLOR_RED, "fatal", __VA_ARGS__)
#define printfc_error(...) printfc(COLOR_RED, "error", __VA_ARGS__)
#define printfc_warn(...) printfc(COLOR_YELLOW, "warning", __VA_ARGS__)
#define printfc_info(...) printfc(COLOR_GREEN, "info", __VA_ARGS__)
#define printfc_critical(...) printfc(COLOR_RED, "critical", __VA_ARGS__)

// TTY printfc
// When the program may not be guaranteed to run in a TTY, use tprintfc instead
// bool istty = isatty(int fd) == 1;

#ifdef NDEBUG
#define tprintfc_debug(istty, ...) ((void)0)
#define tprintfc_trace(istty, ...) ((void)0)
#else // NDEBUG
#define tprintfc_debug(istty, ...) tprintfc(istty, COLOR_CYAN, "debug", __VA_ARGS__)
#define tprintfc_trace(istty, ...) tprintfc(istty, COLOR_MAGENTA, "trace", __VA_ARGS__)
#endif // NDEBUG

#define tprintfc(istty, color, level, ...)                                        \
	do {                                                                          \
		(istty) ? printfc(color, level, __VA_ARGS__) :                            \
				  (fprintf(stderr, "%s: ", level), fprintf(stderr, __VA_ARGS__)); \
	} while (0)
#define tprintfc_fatal(istty, ...) tprintfc(istty, COLOR_RED, "fatal", __VA_ARGS__)
#define tprintfc_error(istty, ...) tprintfc(istty, COLOR_RED, "error", __VA_ARGS__)
#define tprintfc_warn(istty, ...) tprintfc(istty, COLOR_YELLOW, "warning", __VA_ARGS__)
#define tprintfc_info(istty, ...) tprintfc(istty, COLOR_GREEN, "info", __VA_ARGS__)
#define tprintfc_critical(istty, ...) tprintfc(istty, COLOR_RED, "critical", __VA_ARGS__)

#endif // AOC_PRINTFC_H_
