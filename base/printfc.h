#ifndef AOCLIBS_PRINTFC_H_
#define AOCLIBS_PRINTFC_H_

#include "colors.h"
#include <stdio.h>

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

// clang-format off

#ifdef NDEBUG

#ifdef AOCLIBS_STRIP_PREFIX
        #define printfc_debug(...) ((void)0)
        #define printfc_trace(...) ((void)0)
#else
        #define aoc_printfc_debug(...) ((void)0)
        #define aoc_printfc_trace(...) ((void)0)
#endif // AOCLIBS_STRIP_PREFIX

#else // NDEBUG

#ifdef AOCLIBS_STRIP_PREFIX
        #define printfc_debug(fmt, ...) printfc(COLOR_CYAN, "DEBUG", fmt, __VA_ARGS__)
        #define printfc_trace(fmt, ...) printfc(COLOR_MAGENTA, "TRACE", fmt, __VA_ARGS__)
#else
        #define aoc_printfc_debug(fmt, ...) aoc_printfc(COLOR_CYAN, "DEBUG", fmt, __VA_ARGS__)
        #define aoc_printfc_trace(fmt, ...) aoc_printfc(COLOR_MAGENTA, "TRACE", fmt, __VA_ARGS__)
#endif // AOCLIBS_STRIP_PREFIX

#endif // NDEBUG

#ifdef AOCLIBS_STRIP_PREFIX
        #define printfc(color, level, fmt, ...) \
                fprintf(stderr, "%s[%s]%s " fmt, color, level, COLOR_RESET, __VA_ARGS__)
        #define printfc_fatal(fmt, ...) printfc(COLOR_RED, "FATAL", fmt, __VA_ARGS__)
        #define printfc_error(fmt, ...) printfc(COLOR_RED, "ERROR", fmt, __VA_ARGS__)
        #define printfc_warn(fmt, ...) printfc(COLOR_YELLOW, "WARNING", fmt, __VA_ARGS__)
        #define printfc_info(fmt, ...) printfc(COLOR_GREEN, "INFO", fmt, __VA_ARGS__)
        #define printfc_critical(fmt, ...) printfc(COLOR_RED, "CRITICAL", fmt, __VA_ARGS__)
#else
        #define aoc_printfc(color, level, fmt, ...) \
                fprintf(stderr, "%s[%s]%s " fmt, color, level, COLOR_RESET, __VA_ARGS__)
        #define aoc_printfc_fatal(fmt, ...) aoc_printfc(COLOR_RED, "FATAL", fmt, __VA_ARGS__)
        #define aoc_printfc_error(fmt, ...) aoc_printfc(COLOR_RED, "ERROR", fmt, __VA_ARGS__)
        #define aoc_printfc_warn(fmt, ...) aoc_printfc(COLOR_YELLOW, "WARNING", fmt, __VA_ARGS__)
        #define aoc_printfc_info(fmt, ...) aoc_printfc(COLOR_GREEN, "INFO", fmt, __VA_ARGS__)
        #define aoc_printfc_critical(fmt, ...) aoc_printfc(COLOR_RED, "CRITICAL", fmt, __VA_ARGS__)
#endif // AOCLIBS_STRIP_PREFIX

// clang-format on

#endif // AOCLIBS_PRINTFC_H_
