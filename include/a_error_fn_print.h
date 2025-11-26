#ifndef AOCLIBS_ERROR_PRINT_H
#define AOCLIBS_ERROR_PRINT_H

#include <a_error_types.h>
#include <stdio.h>

/*
 * Prints error message
*/
Error print_error(Error error, FILE *output);

/*
 * Prints error message, and appends a new line
*/
Error println_error(Error err, FILE *output);

#endif
