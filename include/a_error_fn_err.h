#ifndef AOCLIBS_ERROR_ERR_H
#define AOCLIBS_ERROR_ERR_H

#include <a_error_types.h>

/*
 * Creates an Error type
*/
Error err(int code, const char msg[]);

#endif
