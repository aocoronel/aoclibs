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
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void printfc_fatal(const char *_Nonnull fmt, ...);

/*
 * Print error formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void printfc_error(const char *_Nonnull fmt, ...);

/*
 * Print warning formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void printfc_warn(const char *_Nonnull fmt, ...);

/*
 * Print info formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void printfc_info(const char *_Nonnull fmt, ...);

/*
 * Print debug formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void printfc_debug(const char *_Nonnull fmt, ...);

/*
 * Print trace formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void printfc_trace(const char *_Nonnull fmt, ...);

/*
 * Print critical formatted message to stderr
 *
 * Defines:
 * - FEATURE_DISABLE_COLORS :: Disable colors
*/
void printfc_critical(const char *_Nonnull fmt, ...);

#endif
