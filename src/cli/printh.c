#include "aoclibs/cli/program_info.h"
#include <aoclibs/cli/printh.h>
#include <aoclibs/io/colors.h>
#include <aoclibs/io/print.h>
#include <aoclibs/mem/str.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*
 * Helper to print headings
*/
static void print_header(const char *msg, const char *style) {
        fprintf(stderr, "%s%s%s", style, msg, COLOR_RESET);
}

/*
 * Helper qsort to sort commands
*/
static int qsort_compare_cmd(const void *a, const void *b) {
        const CLICommand *CMD_A = (const CLICommand *)a;
        const CLICommand *CMD_B = (const CLICommand *)b;

        char fullA[PRINTH_BUFFER], fullB[PRINTH_BUFFER];
        snprintf(fullA, sizeof(fullA), "%s", CMD_A->cmd);
        snprintf(fullB, sizeof(fullB), "%s", CMD_B->cmd);

        return strcmp(fullA, fullB);
}

static const char *qsort_get_opt(const CLIOption *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

/*
 * Helper qsort to sort options
*/
static int qsort_compare_opts(const void *a, const void *b) {
        const CLIOption *FLAG_A = (const CLIOption *)a;
        const CLIOption *FLAG_B = (const CLIOption *)b;

        return strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

static bool has_commands(CLIProgram info) {
        for (int i = 0; i < info.cmdc; i++) {
                if (info.commands->cmd != NULL) return true;
        }
        return false;
}

static bool has_args(CLIProgram info) {
        for (int i = 0; i < info.cmdc; i++) {
                if (info.args->name != NULL) return true;
        }
        return false;
}

static bool has_options(CLIProgram info) {
        for (int i = 0; i < info.flagc; i++) {
                if (info.flags->long_opt != NULL ||
                    info.flags->short_opt != NULL)
                        return true;
        }
        return false;
}

// Arguments:
//   arg1
//       Description
//   arg2
//       Description
static void printh_arguments(CLIProgram *prog) {
        print_header("Arguments:\n", COLOR_BOLD_UNDERLINE);
        for (int i = 0; i < prog->argc; i++) {
                const char *ARG_NAME = prog->args[i].name;
                const char *ARG_DESC = prog->args[i].desc;

                char arg_full[PRINTH_BUFFER] = { 0 };

                if (ARG_NAME) {
                        snprintf(arg_full, sizeof(arg_full), "%s%s%s",
                                 COLOR_BOLD, ARG_NAME, COLOR_RESET);
                        fprintf(stderr, "  %s\n", arg_full);
                        if (ARG_DESC && strlen(ARG_DESC) > 0) {
                                io_print_indent(ARG_DESC, PRINTH_DESC_INDENT);
                                fputc('\n', stderr);
                        }
                }
        }
        fputc('\n', stderr);
}

// Commands:
//   cmd1 <ARG>
//       Description
//   cmd2 [ARG]
//       Description
static void printh_commands(CLIProgram *prog) {
        print_header("Commands:\n", COLOR_BOLD_UNDERLINE);

        for (int i = 0; i < prog->cmdc; i++) {
                const char *CMD = prog->commands[i].cmd;
                const char *ARG = (prog->commands[i].args) ?
                                          prog->commands[i].args->name :
                                          NULL;
                const char *DESC = prog->commands[i].desc;

                char cmd_full[PRINTH_BUFFER] = { 0 };

                if (ARG) {
                        switch (prog->commands[i].args->type) {
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
                if (DESC && strlen(DESC) > 0) {
                        io_print_indent(DESC, PRINTH_DESC_INDENT);
                        fputc('\n', stderr);
                }
        }
        fputc('\n', stderr);
}

// Options:
//   -s, --short [ARG]
//   -l, --long <ARG>
//   -h, --help
//       Description
static void printh_options(CLIProgram *prog) {
        print_header("Options:\n", COLOR_BOLD_UNDERLINE);

        for (int i = 0; i < prog->flagc; i++) {
                const char *SHORT_OPT = prog->flags[i].short_opt;
                const char *LONG_OPT = prog->flags[i].long_opt;
                const char *ARG = (prog->flags[i].args) ?
                                          prog->flags[i].args->name :
                                          NULL;
                const char *DESC = prog->flags[i].desc;

                char flag_buffer[PRINTH_BUFFER] = { 0 };

                if (SHORT_OPT && LONG_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer),
                                 "%s%s%s, %s%s%s", COLOR_BOLD, SHORT_OPT,
                                 COLOR_RESET, COLOR_BOLD, LONG_OPT,
                                 COLOR_RESET);
                } else if (LONG_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s",
                                 COLOR_BOLD, LONG_OPT, COLOR_RESET);
                } else if (SHORT_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s",
                                 COLOR_BOLD, SHORT_OPT, COLOR_RESET);
                }

                if (ARG) {
                        switch (prog->flags->args->type) {
                        case ReqArg:
                                strcat(flag_buffer, " <");
                                strcat(flag_buffer, ARG);
                                strcat(flag_buffer, ">");
                                break;
                        case NReqArg:
                                strcat(flag_buffer, " [");
                                strcat(flag_buffer, ARG);
                                strcat(flag_buffer, "]");
                                break;
                        }
                }

                fprintf(stderr, "  %s\n", flag_buffer);
                if (DESC && strlen(DESC) > 0) {
                        io_print_indent(DESC, PRINTH_DESC_INDENT);
                        fputc('\n', stderr);
                }
        }
}

void _printh(CLIProgram prog) {
        qsort(prog.commands, prog.cmdc, sizeof(CLICommand), qsort_compare_cmd);
        qsort(prog.flags, prog.flagc, sizeof(CLIOption), qsort_compare_opts);

        // program name | program description
        fprintf(stderr, "%s | %s\n\n", prog.name, prog.desc);

        // Usage: program usage
        print_header("Usage:", COLOR_BOLD_UNDERLINE);
        fprintf(stderr, "  %s%s%s %s\n\n", COLOR_BOLD, prog.name, COLOR_RESET,
                prog.usage);

        if (has_args(prog)) printh_arguments(&prog);
        if (has_commands(prog)) printh_commands(&prog);
        if (has_options(prog)) printh_options(&prog);
}
