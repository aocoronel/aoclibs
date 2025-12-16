#ifndef AOCLIBS_CLI_ZSHGEN_H
#define AOCLIBS_CLI_ZSHGEN_H

#include <aoclibs/int.h>
#include <aoclibs/cli/program_info.h>

#define ZSHGEN_ARG_BUFFER 64

#define zshgen(cmds, arguments, options, env, envc) \
    do { \
        _zshgen((CLIProgram){ \
            .flagc  = ARRAY_LEN(options), \
            .cmdc   = ARRAY_LEN(cmds), \
            .argc   = ARRAY_LEN(arguments), \
            .name   = PROGRAM_NAME, \
            .desc   = PROGRAM_DESC, \
            .usage  = PROGRAM_USAGE, \
            .commands = cmds, \
            .args   = arguments, \
            .flags  = options \
        }, env, envc); \
    } while (0)

/*
 * Define the environment variables
 *
 * name, value
*/
typedef struct {
        const char *name;
        const char *value;
} CLIEnv;

/*
 * Generates bash completions for commands, flags and arguments
*/
void _zshgen(CLIProgram prog, const CLIEnv *env, usize envc);
#endif
