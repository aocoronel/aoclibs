#include <a_io_printfc.h>
#include <stdarg.h>
#include <stdio.h>

void printfc(LogLevel level, const char *fmt, ...) {
        const char *color;
        const char *label;

        switch (level) {
        case FATAL:
                color = PRINTFC_COLOR_RED;
                label = "FATAL";
                fflush(stdout);
                break;
        case ERROR:
                color = PRINTFC_COLOR_RED;
                label = "ERROR";
                break;
        case WARN:
                color = PRINTFC_COLOR_YELLOW;
                label = "WARNING";
                break;
        case INFO:
                color = PRINTFC_COLOR_GREEN;
                label = "INFO";
                break;
        case DEBUG:
                color = PRINTFC_COLOR_BLUE;
                label = "DEBUG";
                break;
        }

        va_list args;
        va_start(args, fmt);
        fprintf(stderr, "%s[%s]:%s ", color, label, PRINTFC_COLOR_RESET);
        vfprintf(stderr, fmt, args);
        va_end(args);
}
