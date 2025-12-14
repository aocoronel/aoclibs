#ifndef BASHGEN_H
#define BASHGEN_H

#include <aoclibs/int.h>
#include <aoclibs/cli/program_info.h>

#define bashgen(cmds, arguments, options, env, envc) \
    do { \
        _bashgen((CLIProgram){ \
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
void _bashgen(CLIProgram prog, const CLIEnv *env, usize envc);

#endif // BASHGEN_H
