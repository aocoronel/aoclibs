#ifndef PRINTH_H
#define PRINTH_H

#define PH_BOLD "\x1b[1m"
#define PH_UNDERLINE "\x1b[4m"
#define PH_RESET "\x1b[0m"
#define PH_BOLD_UNDERLINE "\x1b[1;4m"

#define PH_DESC_INDENT 10

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

/*
 * Prints help message
*/
void printh(ProgramInfo program_info);

#endif // PRINTH_H
