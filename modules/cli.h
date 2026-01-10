#ifndef AOCLIBS_CLI_H_
#define AOCLIBS_CLI_H_

#define AOC_CLI_BUFFER 128
#define PRINTH_DESC_INDENT 10 // Indentation by spaces

#include "../base/attributes.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define aoc_cli_program(cmds, arguments, options, env) \
        { .flagc = array_len(options),                 \
          .cmdc = array_len(cmds),                     \
          .argc = array_len(arguments),                \
          .name = PROGRAM_NAME,                        \
          .desc = PROGRAM_DESC,                        \
          .usage = PROGRAM_USAGE,                      \
          .commands = (cmds),                          \
          .args = (arguments),                         \
          .flags = (options) }

int optind = 0;
char *optarg = NULL;
char *optcur = NULL;

typedef struct {
        const char *name;
        const char *desc;
        const char *completion;
} CLIArgument;

typedef struct {
        const char *cmd;
        const CLIArgument *args;
        const char *desc;
} CLICommand;

typedef struct {
        const char *short_opt;
        const char *long_opt;
        const CLIArgument *args;
        const char *desc;
} CLIOption;

typedef struct {
        const char *name;
        const char *value; // HOME, PATH, SHELL...
} CLIEnv;

typedef struct {
        size_t flagc; // Flag count
        size_t cmdc; // Command count
        size_t argc; // Argument count
        const char *name;
        const char *desc;
        const char *usage;
        CLIArgument *args;
        CLICommand *commands;
        CLIOption *flags;
} CLIProgram;

/*
 * Prints indented message
 *
 * If the size of the terminal cannot be obtained, fallback to 80 columns
*/
AOCLIBS_PREFIX void aoc_iprint(const char *msg, int indent);

/*
 * Generates bash completions for commands, flags and arguments
*/
void aoc_bashgen(CLIProgram prog, const CLIEnv *env, size_t envc);

/*
 * Normalizes a strstrg strto a valid shell function name, by replacstrg
 * non-alphanumeric characters with underscores.
 *
 * May truncate to buff_size
*/
void cli_normalize_shell_name(char *buff, const char *str, size_t buff_size);

/*
 * Prints help message
*/
void _aoc_printh(CLIProgram prog);

/*
 * Generates bash completions for commands, flags and arguments
*/
void aoc_zshgen(CLIProgram prog, const CLIEnv *env, size_t envc);

char *get_arg(char *argv[], int argc);

#define ArgMissingOptarg -1
#define ArgNotOpt -2
#define ArgNotDefined -3
#define ArgNoOptAvailable -4
int parse_option(char *argv[], int argc, const CLIProgram *opts);

#endif // AOCLIBS_CLI_H_
