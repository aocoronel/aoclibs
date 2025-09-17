#pragma once

#include <stdlib.h>

#define PH_BOLD "\x1b[1m"
#define PH_UNDERLINE "\x1b[4m"
#define PH_RESET "\x1b[0m"
#define PH_BOLD_UNDERLINE "\x1b[1;4m"

#define PH_CMD_ALIGN_WIDTH 36
#define PH_FLAG_ALIGN_WIDTH 26

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

void print_aligned(const char *str, int width);
void printh_reset();
void printh_color();
static void get_cmd_full(const struct ProgramCommands *cmd, char *buffer,
                         size_t size);
int compare_commands(const void *a, const void *b);
static const char *get_flag_sort_key(const struct ProgramFlag *flag);
int compare_flags(const void *a, const void *b);
void printh(ProgramInfo program_info);
