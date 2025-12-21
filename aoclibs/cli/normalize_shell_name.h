#ifndef AOCLIBS_CLI_NORMALIZE_SHELL_NAME_H
#define AOCLIBS_CLI_NORMALIZE_SHELL_NAME_H

#include <aoclibs/int.h>

/*
 * Normalizes a strstrg strto a valid shell function name, by replacstrg
 * non-alphanumeric characters with underscores.
 *
 * May truncate to buff_size
*/
void cli_normalize_shell_name(char *buff, const char *str, usize buff_size);

#endif
