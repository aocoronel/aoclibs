#pragma once

#include <stdlib.h>

#define PH_BOLD "\x1b[1m"
#define PH_UNDERLINE "\x1b[4m"
#define PH_RESET "\x1b[0m"
#define PH_BOLD_UNDERLINE "\x1b[1;4m"

#define PH_DESC_INDENT 10

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
  int flag_count;
  int command_count;
  const char *prog_name;
  const char *prog_desc;
  const char *prog_usage;
  struct ProgramCommands *commands;
  struct ProgramFlag *flags;
} ProgramInfo;

void printh(ProgramInfo program_info);
