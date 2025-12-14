#include "aoclibs/cli/program_info.h"
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

static void qsort_get_cmd(const CLICommand *cmd, char *buffer, size_t size) {
        if (cmd->args && cmd->args->name) {
                snprintf(buffer, size, "%s %s", cmd->cmd, cmd->args->name);
        } else {
                snprintf(buffer, size, "%s", cmd->cmd);
        }
}

static int qsort_compare_cmd(const void *a, const void *b) {
        const CLICommand *CMD_A = (const CLICommand *)a;
        const CLICommand *CMD_B = (const CLICommand *)b;

        char fullA[PRINTH_CMD_BUFFER], fullB[PRINTH_CMD_BUFFER];
        qsort_get_cmd(CMD_A, fullA, sizeof(fullA));
        qsort_get_cmd(CMD_B, fullB, sizeof(fullB));

        return a_strcmp(fullA, fullB);
}

static const char *qsort_get_opt(const CLIOption *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

static int qsort_compare_opts(const void *a, const void *b) {
        const CLIOption *FLAG_A = (const CLIOption *)a;
        const CLIOption *FLAG_B = (const CLIOption *)b;

        return a_strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

static int has_commands(CLIProgram info) {
        int exist = 0;
        for (int i = 0; i < info.cmdc; i++) {
                if (info.commands->cmd != NULL) exist++;
        }
        return exist;
}

static int has_args(CLIProgram info) {
        int exist = 0;
        for (int i = 0; i < info.cmdc; i++) {
                if (info.args->name != NULL) exist++;
        }
        return exist;
}

static int has_options(CLIProgram info) {
        int exist = 0;
        for (int i = 0; i < info.flagc; i++) {
                if (info.flags->long_opt != NULL ||
                    info.flags->short_opt != NULL)
                        exist++;
        }
        return exist;
}

void _printh(CLIProgram program_info) {
        qsort(program_info.commands, program_info.cmdc, sizeof(CLICommand),
              qsort_compare_cmd);

        qsort(program_info.flags, program_info.flagc, sizeof(CLIOption),
              qsort_compare_opts);

        fprintf(stderr, "%s | %s\n\n", program_info.name, program_info.desc);

        print_header("Usage:", COLOR_BOLD_UNDERLINE);
        fprintf(stderr, "  %s%s%s %s\n\n", COLOR_BOLD, program_info.name,
                COLOR_RESET, program_info.usage);

        if (has_args(program_info) != 0) {
                print_header("Arguments:\n", COLOR_BOLD_UNDERLINE);
                for (int i = 0; i < program_info.argc; i++) {
                        const char *ARG_NAME = program_info.args[i].name;
                        const char *ARG_DESC = program_info.args[i].desc;

                        char arg_full[128] = { 0 };

                        if (ARG_NAME) {
                                snprintf(arg_full, sizeof(arg_full), "%s%s%s",
                                         COLOR_BOLD, ARG_NAME, COLOR_RESET);
                                fprintf(stderr, "  %s\n", arg_full);
                                if (ARG_DESC && a_strlen(ARG_DESC) > 0) {
                                        io_print_indent(ARG_DESC,
                                                        PRINTH_DESC_INDENT);
                                        fputc('\n', stderr);
                                }
                        }
                }
                fputc('\n', stderr);
        }

        if (has_commands(program_info) != 0) {
                print_header("Commands:\n", COLOR_BOLD_UNDERLINE);

                for (int i = 0; i < program_info.cmdc; i++) {
                        const char *CMD = program_info.commands[i].cmd;
                        const char *ARG =
                                (program_info.commands[i].args) ?
                                        program_info.commands[i].args->name :
                                        NULL;
                        const char *DESC = program_info.commands[i].desc;

                        char cmd_full[128] = { 0 };

                        if (ARG) {
                                switch (program_info.commands[i].args->type) {
                                case ReqArg:
                                        snprintf(cmd_full, sizeof(cmd_full),
                                                 "%s%s%s <%s>", COLOR_BOLD, CMD,
                                                 COLOR_RESET, ARG);
                                        break;
                                case NReqArg:
                                        snprintf(cmd_full, sizeof(cmd_full),
                                                 "%s%s%s [%s]", COLOR_BOLD, CMD,
                                                 COLOR_RESET, ARG);
                                        break;
                                }
                        } else {
                                snprintf(cmd_full, sizeof(cmd_full), "%s%s%s",
                                         COLOR_BOLD, CMD, COLOR_RESET);
                        }

                        fprintf(stderr, "  %s\n", cmd_full);
                        if (DESC && a_strlen(DESC) > 0) {
                                io_print_indent(DESC, PRINTH_DESC_INDENT);
                                fputc('\n', stderr);
                        }
                }
                fputc('\n', stderr);
        }

        if (has_options(program_info) != 0) {
                print_header("Options:\n", COLOR_BOLD_UNDERLINE);

                for (int i = 0; i < program_info.flagc; i++) {
                        const char *SHORT_OPT = program_info.flags[i].short_opt;
                        const char *LONG_OPT = program_info.flags[i].long_opt;
                        const char *ARG =
                                (program_info.flags[i].args) ?
                                        program_info.flags[i].args->name :
                                        NULL;
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
                                switch (program_info.flags->args->type) {
                                case ReqArg:
                                        a_strcat(flag_buffer, " <");
                                        a_strcat(flag_buffer, ARG);
                                        a_strcat(flag_buffer, ">");
                                        break;
                                case NReqArg:
                                        a_strcat(flag_buffer, " [");
                                        a_strcat(flag_buffer, ARG);
                                        a_strcat(flag_buffer, "]");
                                        break;
                                }
                        }

                        fprintf(stderr, "  %s\n", flag_buffer);
                        if (DESC && a_strlen(DESC) > 0) {
                                io_print_indent(DESC, PRINTH_DESC_INDENT);
                                fputc('\n', stderr);
                        }
                }
        }
}
