#include "printh.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <aoclibs/io/print.h>
#include <aoclibs/mem/str.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define PRINTH_CMD_BUFFER 256

static void print_header(const char *msg, const char *style) {
        fprintf(stderr, "%s%s%s", style, msg, PRINTH_RESET);
}

static void get_cmd_full(const CmdMetadata *cmd, char *buffer, size_t size) {
        if (cmd->args) {
                snprintf(buffer, size, "%s %s", cmd->cmd, cmd->args);
        } else {
                snprintf(buffer, size, "%s", cmd->cmd);
        }
}

int compare_commands(const void *a, const void *b) {
        const CmdMetadata *cmdA = (const CmdMetadata *)a;
        const CmdMetadata *cmdB = (const CmdMetadata *)b;

        char fullA[PRINTH_CMD_BUFFER], fullB[PRINTH_CMD_BUFFER];
        get_cmd_full(cmdA, fullA, sizeof(fullA));
        get_cmd_full(cmdB, fullB, sizeof(fullB));

        return a_strcmp(fullA, fullB);
}

static const char *get_opt_sort_key(const OptionMetadata *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

static int compare_opts(const void *a, const void *b) {
        const OptionMetadata *flagA = (const OptionMetadata *)a;
        const OptionMetadata *flagB = (const OptionMetadata *)b;

        return a_strcmp(get_opt_sort_key(flagA), get_opt_sort_key(flagB));
}

int has_commands(ProgramInfo info) {
        int exist = 0;
        for (int i = 0; i < info.cmdc; i++) {
                if (info.commands->cmd != NULL) exist++;
        }
        return exist;
}

int has_options(ProgramInfo info) {
        int exist = 0;
        for (int i = 0; i < info.flagc; i++) {
                if (info.flags->long_opt != NULL ||
                    info.flags->short_opt != NULL)
                        exist++;
        }
        return exist;
}

void printh(ProgramInfo program_info) {
        FILE *out = stderr;

        qsort(program_info.commands, program_info.cmdc, sizeof(CmdMetadata),
              compare_commands);

        qsort(program_info.flags, program_info.flagc, sizeof(OptionMetadata),
              compare_opts);

        fprintf(out, "%s | %s\n\n", program_info.name, program_info.desc);

        print_header("Usage:", PRINTH_BOLD_UNDERLINE);
        fprintf(out, "  %s%s%s %s\n\n", PRINTH_BOLD, program_info.name,
                PRINTH_RESET, program_info.usage);

        if (has_commands(program_info) != 0) {
                print_header("Commands:\n", PRINTH_BOLD_UNDERLINE);

                for (int i = 0; i < program_info.cmdc; i++) {
                        const char *cmd = program_info.commands[i].cmd;
                        const char *arg = program_info.commands[i].args;
                        const char *desc = program_info.commands[i].desc;

                        char cmd_full[128] = { 0 };

                        if (arg) {
                                snprintf(cmd_full, sizeof(cmd_full),
                                         "%s%s%s %s", PRINTH_BOLD, cmd,
                                         PRINTH_RESET, arg);
                        } else {
                                snprintf(cmd_full, sizeof(cmd_full), "%s%s%s",
                                         PRINTH_BOLD, cmd, PRINTH_RESET);
                        }

                        fprintf(out, "  %s\n", cmd_full);
                        if (desc && a_strlen(desc) > 0) {
                                io_print_indent(desc, PRINTH_DESC_INDENT);
                                fputc('\n', stdout);
                        }
                }
                fputc('\n', stdout);
        }

        if (has_options(program_info) != 0) {
                print_header("Options:\n", PRINTH_BOLD_UNDERLINE);

                for (int i = 0; i < program_info.flagc; i++) {
                        const char *short_opt = program_info.flags[i].short_opt;
                        const char *long_opt = program_info.flags[i].long_opt;
                        const char *arg = program_info.flags[i].args;
                        const char *desc = program_info.flags[i].desc;

                        char flag_buffer[128] = { 0 };

                        if (short_opt && long_opt) {
                                snprintf(flag_buffer, sizeof(flag_buffer),
                                         "%s%s%s, %s%s%s", PRINTH_BOLD,
                                         short_opt, PRINTH_RESET, PRINTH_BOLD,
                                         long_opt, PRINTH_RESET);
                        } else if (long_opt) {
                                snprintf(flag_buffer, sizeof(flag_buffer),
                                         "%s%s%s", PRINTH_BOLD, long_opt,
                                         PRINTH_RESET);
                        } else if (short_opt) {
                                snprintf(flag_buffer, sizeof(flag_buffer),
                                         "%s%s%s", PRINTH_BOLD, short_opt,
                                         PRINTH_RESET);
                        }

                        if (arg) {
                                char upper_arg[32] = { 0 };
                                for (int j = 0; arg[j] && j < 30; j++) {
                                        upper_arg[j] = (char)toupper(arg[j]);
                                }

                                a_strcat(flag_buffer, " <");
                                a_strcat(flag_buffer, upper_arg);
                                a_strcat(flag_buffer, ">");
                        }

                        fprintf(out, "  %s\n", flag_buffer);
                        if (desc && a_strlen(desc) > 0) {
                                io_print_indent(desc, PRINTH_DESC_INDENT);
                                fputc('\n', stdout);
                        }
                }
        }
}
