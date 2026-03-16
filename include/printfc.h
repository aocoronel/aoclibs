#ifndef AOCLIBS_PRINTFC_H_
#define AOCLIBS_PRINTFC_H_

#include "colors.h"

#ifdef AOCLIBS_IMPLEMENTATION
#include <stdio.h>
#endif

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
#define aoc_printfc_debug(...) ((void)0)
#define aoc_printfc_trace(...) ((void)0)
#else // NDEBUG
#define aoc_printfc_debug(...) aoc_printfc(COLOR_CYAN, "DEBUG", __VA_ARGS__)
#define aoc_printfc_trace(...) aoc_printfc(COLOR_MAGENTA, "TRACE", __VA_ARGS__)
#endif // NDEBUG

#define aoc_printfc(color, level, ...) \
    (fprintf(stderr, "%s[%s]%s ", color, level, COLOR_RESET), fprintf(stderr, __VA_ARGS__))
#define aoc_printfc_fatal(...) aoc_printfc(COLOR_RED, "FATAL", __VA_ARGS__)
#define aoc_printfc_error(...) aoc_printfc(COLOR_RED, "ERROR", __VA_ARGS__)
#define aoc_printfc_warn(...) aoc_printfc(COLOR_YELLOW, "WARNING", __VA_ARGS__)
#define aoc_printfc_info(...) aoc_printfc(COLOR_GREEN, "INFO", __VA_ARGS__)
#define aoc_printfc_critical(...) aoc_printfc(COLOR_RED, "CRITICAL", __VA_ARGS__)

#ifdef AOCLIBS_STRIP_PREFIX
#define printfc aoc_printfc
#define printfc_fatal aoc_printfc_fatal
#define printfc_error aoc_printfc_error
#define printfc_warn aoc_printfc_warn
#define printfc_info aoc_printfc_info
#define printfc_critical aoc_printfc_critical
#define printfc_debug aoc_printfc_debug
#define printfc_trace aoc_printfc_trace
#endif // AOCLIBS_STRIP_PREFIX

#endif // AOCLIBS_PRINTFC_H_
