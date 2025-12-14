#include <aoclibs/cli/printh.h>
#include <aoclibs/io/colors.h>
#include <aoclibs/io/print.h>
#include <aoclibs/mem/str.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

static void print_header(const char *msg, const char *style) {
        fprintf(stderr, "%s%s%s", style, msg, COLOR_RESET);
}

static void qsort_get_cmd(const CmdMetadata *cmd, char *buffer, size_t size) {
        if (cmd->args) {
                snprintf(buffer, size, "%s %s", cmd->cmd, cmd->args);
        } else {
                snprintf(buffer, size, "%s", cmd->cmd);
        }
}

static int qsort_compare_cmd(const void *a, const void *b) {
        const CmdMetadata *CMD_A = (const CmdMetadata *)a;
        const CmdMetadata *CMD_B = (const CmdMetadata *)b;

        char fullA[PRINTH_CMD_BUFFER], fullB[PRINTH_CMD_BUFFER];
        qsort_get_cmd(CMD_A, fullA, sizeof(fullA));
        qsort_get_cmd(CMD_B, fullB, sizeof(fullB));

        return a_strcmp(fullA, fullB);
}

static const char *qsort_get_opt(const OptionMetadata *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

static int qsort_compare_opts(const void *a, const void *b) {
        const OptionMetadata *FLAG_A = (const OptionMetadata *)a;
        const OptionMetadata *FLAG_B = (const OptionMetadata *)b;

        return a_strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

static int has_commands(ProgramInfo info) {
        int exist = 0;
        for (int i = 0; i < info.cmdc; i++) {
                if (info.commands->cmd != NULL) exist++;
        }
        return exist;
}

static int has_options(ProgramInfo info) {
        int exist = 0;
        for (int i = 0; i < info.flagc; i++) {
                if (info.flags->long_opt != NULL ||
                    info.flags->short_opt != NULL)
                        exist++;
        }
        return exist;
}

void printh(ProgramInfo program_info) {
        qsort(program_info.commands, program_info.cmdc, sizeof(CmdMetadata),
              qsort_compare_cmd);

        qsort(program_info.flags, program_info.flagc, sizeof(OptionMetadata),
              qsort_compare_opts);

        fprintf(stderr, "%s | %s\n\n", program_info.name, program_info.desc);

        print_header("Usage:", COLOR_BOLD_UNDERLINE);
        fprintf(stderr, "  %s%s%s %s\n\n", COLOR_BOLD, program_info.name,
                COLOR_RESET, program_info.usage);

        if (has_commands(program_info) != 0) {
                print_header("Commands:\n", COLOR_BOLD_UNDERLINE);

                for (int i = 0; i < program_info.cmdc; i++) {
                        const char *CMD = program_info.commands[i].cmd;
                        const char *ARG = program_info.commands[i].args;
                        const char *DESC = program_info.commands[i].desc;

                        char cmd_full[128] = { 0 };

                        if (ARG) {
                                snprintf(cmd_full, sizeof(cmd_full),
                                         "%s%s%s %s", COLOR_BOLD, CMD,
                                         COLOR_RESET, ARG);
                        } else {
                                snprintf(cmd_full, sizeof(cmd_full), "%s%s%s",
                                         COLOR_BOLD, CMD, COLOR_RESET);
                        }

                        fprintf(stderr, "  %s\n", cmd_full);
                        if (DESC && a_strlen(DESC) > 0) {
                                io_print_indent(DESC, PRINTH_DESC_INDENT);
                                fputc('\n', stdout);
                        }
                }
                fputc('\n', stdout);
        }

        if (has_options(program_info) != 0) {
                print_header("Options:\n", COLOR_BOLD_UNDERLINE);

                for (int i = 0; i < program_info.flagc; i++) {
                        const char *SHORT_OPT = program_info.flags[i].short_opt;
                        const char *LONG_OPT = program_info.flags[i].long_opt;
                        const char *ARG = program_info.flags[i].args;
                        const char *DESC = program_info.flags[i].desc;

                        char flag_buffer[128] = { 0 };

                        if (SHORT_OPT && LONG_OPT) {
                                snprintf(flag_buffer, sizeof(flag_buffer),
                                         "%s%s%s, %s%s%s", COLOR_BOLD,
                                         SHORT_OPT, COLOR_RESET, COLOR_BOLD,
                                         LONG_OPT, COLOR_RESET);
                        } else if (LONG_OPT) {
                                snprintf(flag_buffer, sizeof(flag_buffer),
                                         "%s%s%s", COLOR_BOLD, LONG_OPT,
                                         COLOR_RESET);
                        } else if (SHORT_OPT) {
                                snprintf(flag_buffer, sizeof(flag_buffer),
                                         "%s%s%s", COLOR_BOLD, SHORT_OPT,
                                         COLOR_RESET);
                        }

                        if (ARG) {
                                char upper_arg[32] = { 0 };
                                for (int j = 0; ARG[j] && j < 30; j++) {
                                        upper_arg[j] = (char)toupper(ARG[j]);
                                }

                                a_strcat(flag_buffer, " <");
                                a_strcat(flag_buffer, upper_arg);
                                a_strcat(flag_buffer, ">");
                        }

                        fprintf(stderr, "  %s\n", flag_buffer);
                        if (DESC && a_strlen(DESC) > 0) {
                                io_print_indent(DESC, PRINTH_DESC_INDENT);
                                fputc('\n', stdout);
                        }
                }
        }
}
