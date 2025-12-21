#ifndef AOCLIBS_CLI_PRINTH_H
#define AOCLIBS_CLI_PRINTH_H

#include <aoclibs/cli/program_info.h>

#define PRINTH_DESC_INDENT 10
#define PRINTH_BUFFER 128

#define cli_printh(cmds, arguments, options) \
    do { \
        _cli_printh((CLIProgram){ \
            .flagc  = array_len(options), \
            .cmdc   = array_len(cmds), \
            .argc   = array_len(arguments), \
            .name   = PROGRAM_NAME, \
            .desc   = PROGRAM_DESC, \
            .usage  = PROGRAM_USAGE, \
            .commands = cmds, \
            .args   = arguments, \
            .flags  = options \
        }); \
    } while (0)

/*
 * Prints help message
*/
void _cli_printh(CLIProgram prog);

#endif
