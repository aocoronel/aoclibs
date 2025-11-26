#ifndef AOCLIBS_ERROR_PANIC_H
#define AOCLIBS_ERROR_PANIC_H

#undef panic

/*
 * Used to panic, when an unreachable code runs
*/
#define unreachable() _panic(__FILE__, __LINE__, __func__, "unreachable code")
#define panic(msg) _panic(__FILE__, __LINE__, __func__, msg)

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use panic, instead.
*/
_Noreturn void _panic(const char *file, int line, const char *func, const char *msg);

#endif
