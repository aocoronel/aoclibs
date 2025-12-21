#ifndef AOCLIBS_CLI_ZSHGEN_H
#define AOCLIBS_CLI_ZSHGEN_H

#include <aoclibs/int.h>
#include <aoclibs/cli/program_info.h>

#define cli_zshgen(cmds, arguments, options, env, envc) \
    do { \
        _cli_zshgen((CLIProgram){ \
            .flagc  = array_len(options), \
            .cmdc   = array_len(cmds), \
            .argc   = array_len(arguments), \
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
void _cli_zshgen(CLIProgram prog, const CLIEnv *env, usize envc);
#endif
