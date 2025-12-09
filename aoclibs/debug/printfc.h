#ifndef AOCLIBS_IO_PRINTFC_H
#define AOCLIBS_IO_PRINTFC_H

#ifdef NDEBUG
#define pdbg(...) ((void)0)
#define ptrace(...) ((void)0)
#else
#define pdbg(...) printfc(DEBUG, __VA_ARGS__)
#define ptrace(...) printfc(TRACE, __VA_ARGS__)
#endif // NDEBUG

typedef enum {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        CRITICAL,
        FATAL
} LogLevel;

/*
 * Print colored and formatted messages to stderr, based on LogLevel
*/
void printfc(LogLevel level, const char *fmt, ...);

#endif
