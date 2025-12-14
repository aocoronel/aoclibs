#ifndef AOCLIBS_CLI_PRINTH_H
#define AOCLIBS_CLI_PRINTH_H

#include <aoclibs/cli/program_info.h>

#define PRINTH_DESC_INDENT 10
#define PRINTH_CMD_BUFFER 256

/*
 * Prints help message
*/
void printh(ProgramInfo program_info);

#endif
