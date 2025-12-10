#include <aoclibs/io/colors.h>
#include <aoclibs/debug/printfc.h>
#include <stdarg.h>
#include <stdio.h>

void printfc(LogLevel level, const char *fmt, ...) {
        const char *COLOR;
        const char *LABEL;

        switch (level) {
        case FATAL:
                COLOR = COLOR_RED;
                LABEL = "FATAL";
                break;
        case ERROR:
                COLOR = COLOR_RED;
                LABEL = "ERROR";
                break;
        case WARN:
                COLOR = COLOR_YELLOW;
                LABEL = "WARNING";
                break;
        case INFO:
                COLOR = COLOR_GREEN;
                LABEL = "INFO";
                break;
        case DEBUG:
                COLOR = COLOR_CYAN;
                LABEL = "DEBUG";
                break;
        case TRACE:
                COLOR = COLOR_MAGENTA;
                LABEL = "TRACE";
                break;
        case CRITICAL:
                COLOR = COLOR_RED;
                LABEL = "CRITICAL";
                break;
        }

        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", COLOR, LABEL, COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}
