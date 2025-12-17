#include <aoclibs/io/colors.h>
#include <aoclibs/debug/printfc.h>
#include <stdarg.h>
#include <stdio.h>

void printfc_fatal(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_RED, "FATAL", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void printfc_error(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_RED, "ERROR", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void printfc_warn(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_YELLOW, "WARNING", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void printfc_info(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_GREEN, "INFO", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void printfc_debug(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_CYAN, "DEBUG", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void printfc_trace(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_MAGENTA, "TRACE", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}

void printfc_critical(const char *fmt, ...) {
        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR_RED, "CRITICAL", COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}
