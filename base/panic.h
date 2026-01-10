#ifndef AOCLIBS_PANIC_H_
#define AOCLIBS_PANIC_H_

/*
 * Used to panic, when an unreachable code is ran
*/
#define _unreachable aoc_panic(__FILE__, __LINE__, __func__, "unreachable code")

/*
 * Prints message, including metadata and aborts the program
*/
#define panic(msg) aoc_panic(__FILE__, __LINE__, __func__, msg)

/*
 * Prints message, including metadata and aborts the program
 *
 * Should not be used directly. Use the panic macro, instead.
*/
void aoc_panic(const char *__file, int __line, const char *__func, const char *msg) __attribute__((noreturn));

#endif // AOCLIBS_PANIC_H_
