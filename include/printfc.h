#ifndef AOCLIBS_PRINTFC_H_
#define AOCLIBS_PRINTFC_H_

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
#define printfc_debug(...) printfc(COLOR_CYAN, "DEBUG", __VA_ARGS__)
#define printfc_trace(...) printfc(COLOR_MAGENTA, "TRACE", __VA_ARGS__)
#endif // NDEBUG

#define printfc(color, level, ...) \
    (fprintf(stderr, "%s[%s]%s ", color, level, COLOR_RESET), fprintf(stderr, __VA_ARGS__))
#define printfc_fatal(...) printfc(COLOR_RED, "FATAL", __VA_ARGS__)
#define printfc_error(...) printfc(COLOR_RED, "ERROR", __VA_ARGS__)
#define printfc_warn(...) printfc(COLOR_YELLOW, "WARNING", __VA_ARGS__)
#define printfc_info(...) printfc(COLOR_GREEN, "INFO", __VA_ARGS__)
#define printfc_critical(...) printfc(COLOR_RED, "CRITICAL", __VA_ARGS__)

#endif // AOCLIBS_PRINTFC_H_
