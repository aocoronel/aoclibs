#ifndef AOCLIBS_IO_PRINTFC_H
#define AOCLIBS_IO_PRINTFC_H

#ifdef NDEBUG
#define PDBG(...) ((void)0)
#define PTRACE(...) ((void)0)
#else
#define PDBG(...) printfc_debug(__VA_ARGS__)
#define PTRACE(...) printfc_trace(__VA_ARGS__)
#endif // NDEBUG

/*
 * Print fatal formatted message to stderr
*/
void printfc_fatal(const char *fmt, ...);

/*
 * Print error formatted message to stderr
*/
void printfc_error(const char *fmt, ...);

/*
 * Print warning formatted message to stderr
*/
void printfc_warn(const char *fmt, ...);

/*
 * Print info formatted message to stderr
*/
void printfc_info(const char *fmt, ...);

/*
 * Print debug formatted message to stderr
*/
void printfc_debug(const char *fmt, ...);

/*
 * Print trace formatted message to stderr
*/
void printfc_trace(const char *fmt, ...);

/*
 * Print critical formatted message to stderr
*/
void printfc_critical(const char *fmt, ...);

#endif
