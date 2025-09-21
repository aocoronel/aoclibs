#ifndef BASHGEN_H
#define BASHGEN_H

#ifndef PPROGRAM_VAR
#define PPROGRAM_VAR
/*
 * Define the program command details
 *
 * command, argument, description
 */
struct PProgramCommands {
  const char *cmd;
  const char *argument;
  const char *cmd_desc;
};

/*
 * Define the program flag details
 *
 * short flag, long flag, argument, description
 */
struct PProgramFlag {
        const char *short_flag;
        const char *long_flag;
        const char *argument;
        const char *desc;
};

/*
 * Define the program info
 *
 * flag count, command count, program name, program description, program usage,
 * PProgramCommands, PProgramFlag
 */
typedef struct {
        int flagc;
        int cmdc;
        const char *name;
        const char *desc;
        const char *usage;
        struct PProgramCommands *commands;
        struct PProgramFlag *flags;
} PProgramInfo;
#endif // !PPROGRAM_VAR

/*
 * Define the program arguments and completions
 *
 * argument, completion
*/
struct AProgramArguments {
        const char *name;
        const char *completions;
};

/*
 * Includes the program info and program arguments
 *
 * PProgramInfo, AProgramArguments
*/
typedef struct {
        PProgramInfo *info;
        int argc;
        struct AProgramArguments *args;
} CompletionInfo;

/*
 * Generates bash completions for commands, flags and arguments
*/
void generate_bash_completion(const CompletionInfo *info);
#endif // BASHGEN_H
