#include "printh.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PH_DESC_INDENT 10

void print_aligned(const char *str, int width) { printf("%-*s", width, str); }

void printh_p(const char *msg, const char *style) {
  FILE *out = stdout;
  fprintf(out, "%s%s%s", style, msg, PH_RESET);
}

static void get_cmd_full(const struct ProgramCommands *cmd, char *buffer,
                         size_t size) {
  if (cmd->argument) {
    snprintf(buffer, size, "%s %s", cmd->cmd, cmd->argument);
  } else {
    snprintf(buffer, size, "%s", cmd->cmd);
  }
}

int compare_commands(const void *a, const void *b) {
  const struct ProgramCommands *cmdA = (const struct ProgramCommands *)a;
  const struct ProgramCommands *cmdB = (const struct ProgramCommands *)b;

  char fullA[128], fullB[128];
  get_cmd_full(cmdA, fullA, sizeof(fullA));
  get_cmd_full(cmdB, fullB, sizeof(fullB));

  return strcmp(fullA, fullB);
}

static const char *get_flag_sort_key(const struct ProgramFlag *flag) {
  if (flag->short_flag)
    return flag->short_flag;
  if (flag->long_flag)
    return flag->long_flag;
  return "";
}

int compare_flags(const void *a, const void *b) {
  const struct ProgramFlag *flagA = (const struct ProgramFlag *)a;
  const struct ProgramFlag *flagB = (const struct ProgramFlag *)b;

  return strcmp(get_flag_sort_key(flagA), get_flag_sort_key(flagB));
}

void printh(ProgramInfo program_info) {
  FILE *out = stdout;

  qsort(program_info.commands, program_info.command_count,
        sizeof(struct ProgramCommands), compare_commands);

  qsort(program_info.flags, program_info.flag_count, sizeof(struct ProgramFlag),
        compare_flags);

  fprintf(out, "%s | %s\n\n", program_info.prog_name, program_info.prog_desc);

  printh_p("Usage:", PH_BOLD_UNDERLINE);
  fprintf(out, "  %s%s%s", PH_BOLD, program_info.prog_name, PH_RESET);
  fprintf(out, " %s\n\n", program_info.prog_usage);

  printh_p("Commands:\n", PH_BOLD_UNDERLINE);

  for (int i = 0; i < program_info.command_count; i++) {
    const char *cmd = program_info.commands[i].cmd;
    const char *arg = program_info.commands[i].argument;
    const char *desc = program_info.commands[i].cmd_desc;

    char cmd_full[128] = {0};

    if (arg) {
      snprintf(cmd_full, sizeof(cmd_full), "%s%s%s %s", PH_BOLD, cmd, PH_RESET,
               arg);
    } else {
      snprintf(cmd_full, sizeof(cmd_full), "%s%s%s", PH_BOLD, cmd, PH_RESET);
    }

    // Print the command line
    fprintf(out, "  %s\n", cmd_full);

    // Print the description line (indented)
    if (desc && strlen(desc) > 0) {
      fprintf(out, "%*s%s\n", PH_DESC_INDENT, "", desc);
    }
  }
  fprintf(out, "\n");

  printh_p("Options:\n", PH_BOLD_UNDERLINE);

  for (int i = 0; i < program_info.flag_count; i++) {
    const char *short_flag = program_info.flags[i].short_flag;
    const char *long_flag = program_info.flags[i].long_flag;
    const char *arg = program_info.flags[i].argument;
    const char *desc = program_info.flags[i].desc;

    // Build the flag line: "-x, --example" or just "--example"
    char flag_buffer[128] = {0};

    if (short_flag && long_flag) {
      snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s, %s%s%s", PH_BOLD,
               short_flag, PH_RESET, PH_BOLD, long_flag, PH_RESET);
    } else if (long_flag) {
      snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s", PH_BOLD, long_flag,
               PH_RESET);
    } else if (short_flag) {
      snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s", PH_BOLD, short_flag,
               PH_RESET);
    }

    // Add argument if present
    if (arg) {
      char upper_arg[32] = {0};
      for (int j = 0; arg[j] && j < 30; j++) {
        upper_arg[j] = (char)toupper(arg[j]);
      }

      // Append " <ARG>" to flag_buffer
      strncat(flag_buffer, " <", sizeof(flag_buffer) - strlen(flag_buffer) - 1);
      strncat(flag_buffer, upper_arg,
              sizeof(flag_buffer) - strlen(flag_buffer) - 1);
      strncat(flag_buffer, ">", sizeof(flag_buffer) - strlen(flag_buffer) - 1);
    }

    // Print the flag line
    fprintf(out, "  %s\n", flag_buffer);

    // Print the description line if available
    if (desc && strlen(desc) > 0) {
      fprintf(out, "%*s%s\n", PH_DESC_INDENT, "", desc);
    }
  }
}
