#ifndef ZSHGEN_H
#define ZSHGEN_H

#ifndef PROGRAM_VAR
#define PROGRAM_VAR
/*
 * Define the program command details
 *
 * command, argument, description
 */
struct ProgramCommands {
  const char *cmd;
  const char *argument;
  const char *cmd_desc;
};

/*
 * Define the program flag details
 *
 * short flag, long flag, argument, description
 */
struct ProgramFlag {
        const char *short_flag;
        const char *long_flag;
        const char *argument;
        const char *desc;
};

/*
 * Define the program info
 *
 * flag count, command count, program name, program description, program usage,
 * ProgramCommands, ProgramFlag
 */
typedef struct {
        int flagc;
        int cmdc;
        const char *name;
        const char *desc;
        const char *usage;
        struct ProgramCommands *commands;
        struct ProgramFlag *flags;
} ProgramInfo;
#endif // !PROGRAM_VAR

#ifndef BZ_COMPLETION
#define BZ_COMPLETION
/*
 * Define the program arguments and completions
 *
 * argument, completion
*/
struct ProgramArguments {
        const char *name;
        const char *completions;
};

/*
 * Define the environment variables
 *
 * name, value
*/
struct ProgramEnv {
        const char *name;
        const char *value;
};

/*
 * Includes the program info and program arguments
 *
 * ProgramInfo, ProgramArguments
*/
typedef struct {
        ProgramInfo *info;
        int argc;
        int envc;
        struct ProgramArguments *args;
        struct ProgramEnv *envs;
} CompletionInfo;
#endif // !BZ_COMPLETION

/*
 * Generates bash completions for commands, flags and arguments
*/
void generate_zsh_completion(const CompletionInfo *info);
#endif // ZSHGEN_H
