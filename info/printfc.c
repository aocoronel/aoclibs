#include "printfc.h"
#include <stdarg.h>
#include <stdio.h>

#define PFATAL(...) printfc(FATAL, __VA_ARGS__)
#define PERROR(...) printfc(ERROR, __VA_ARGS__)
#define PWARN(...) printfc(WARN, __VA_ARGS__)
#define PINFO(...) printfc(INFO, __VA_ARGS__)
#define PDEBUG(...) printfc(DEBUG, __VA_ARGS__)

int printfc(LogLevel level, const char *restrict fmt, ...) {
        const char *color;
        const char *label;
        FILE *out = stdout;

        switch (level) {
        case FATAL:
                color = COLOR_RED;
                label = "FATAL";
                out = stderr;
                break;
        case ERROR:
                color = COLOR_RED;
                label = "ERROR";
                out = stderr;
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
                color = COLOR_BLUE;
                label = "DEBUG";
                break;
        default:
                return 1;
        }

        va_list args;
        va_start(args, fmt);
        fprintf(out, "%s[%s]:%s ", color, label, COLOR_RESET);
        vfprintf(out, fmt, args);
        va_end(args);
        return 0;
}
