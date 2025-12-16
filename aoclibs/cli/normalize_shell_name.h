#ifndef AOCLIBS_CLI_NORMALIZE_SHELL_NAME_H
#define AOCLIBS_CLI_NORMALIZE_SHELL_NAME_H

#include <aoclibs/int.h>

/**
 * Normalizes a string into a valid shell function name.
 * Replaces non-alphanumeric characters with underscores.
 * Ensures the result starts with a letter or underscore.
 * Truncates to max_len if necessary.
 *
 * @param out      Output buffer (must be at least max_len+1 bytes)
 * @param in       Input string to normalize
 * @param max_len  Maximum length of output (excluding NUL terminator)
 */
void cli_normalize_shell_name(char *buff, const char *str, usize buff_size);

#endif
