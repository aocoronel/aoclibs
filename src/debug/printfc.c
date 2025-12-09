#include <aoclibs/io/colors.h>
#include <aoclibs/debug/printfc.h>
#include <stdarg.h>
#include <stdio.h>

void printfc(LogLevel level, const char *fmt, ...) {
        const char *color;
        const char *label;

        switch (level) {
        case FATAL:
                color = COLOR_RED;
                label = "FATAL";
                break;
        case ERROR:
                color = COLOR_RED;
                label = "ERROR";
                break;
        case WARN:
                color = COLOR_YELLOW;
                label = "WARNING";
                break;
        case INFO:
                color = COLOR_GREEN;
                label = "INFO";
                break;
        case DEBUG:
                color = COLOR_CYAN;
                label = "DEBUG";
                break;
        case TRACE:
                color = COLOR_MAGENTA;
                label = "TRACE";
                break;
        case CRITICAL:
                color = COLOR_RED;
                label = "CRITICAL";
                break;
        }

        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", color, label, COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}
