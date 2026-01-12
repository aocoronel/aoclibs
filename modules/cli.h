#ifndef AOCLIBS_CLI_H_
#define AOCLIBS_CLI_H_

#include "../base/attributes.h"
#include "../base/colors.h"
#include "../base/str.h"
#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define AOC_CLI_BUFFER 128
#define AOC_PRINTH_DESC_INDENT 10 // Indentation by spaces

#define aoc_cli_program(cmds, arguments, options, env) \
        { .flagc = array_len(options),                 \
          .cmdc = array_len(cmds),                     \
          .argc = array_len(arguments),                \
          .name = PROGRAM_NAME,                        \
          .desc = PROGRAM_DESC,                        \
          .usage = PROGRAM_USAGE,                      \
          .commands = (cmds),                          \
          .args = (arguments),                         \
          .flags = (options) }

#ifdef AOCLIBS_STRIP_PREFIX
#define iprint aoc_iprint
#define bashgen aoc_bashgen
#define normalize_shell_name aoc_normalize_shell_name
#define printh aoc_printh
#define zshgen aoc_zshgen
#define getarg aoc_getarg
#define getopt aoc_getopt
#endif

int optind = 0; // == getopt
char *optarg = NULL; // == getopt
char *optcur = NULL; // opt current

typedef struct {
        const char *name;
        const char *desc;
        const char *completion;
} CLIArgument;

typedef struct {
        const char *cmd;
        const CLIArgument *args;
        const char *desc;
} CLICommand;

typedef struct {
        const char *short_opt;
        const char *long_opt;
        const CLIArgument *args;
        const char *desc;
} CLIOption;

typedef struct {
        const char *name;
        const char *value; // HOME, PATH, SHELL...
} CLIEnv;

typedef struct {
        size_t flagc; // Flag count
        size_t cmdc; // Command count
        size_t argc; // Argument count
        const char *name;
        const char *desc;
        const char *usage;
        CLIArgument *args;
        CLICommand *commands;
        CLIOption *flags;
} CLIProgram;

/*
 * Prints indented message
 *
 * If the size of the terminal cannot be obtained, fallback to 80 columns
*/
AOCLIBS_PREFIX void aoc_iprint(const char *msg, int indent);

/*
 * Generates bash completions for commands, flags and arguments
*/
AOCLIBS_PREFIX void aoc_bashgen(CLIProgram prog, const CLIEnv *env, size_t envc);

/*
 * Normalizes a strstrg strto a valid shell function name, by replacstrg
 * non-alphanumeric characters with underscores.
 *
 * May truncate to buff_size
*/
AOCLIBS_PREFIX void aoc_normalize_shell_name(char *buff, const char *str, size_t buff_size);

/*
 * Prints help message
*/
AOCLIBS_PREFIX void aoc_printh(CLIProgram prog);

/*
 * Generates bash completions for commands, flags and arguments
*/
AOCLIBS_PREFIX void aoc_zshgen(CLIProgram prog, const CLIEnv *env, size_t envc);

/*
 * Get next argument from argv
*/
AOCLIBS_PREFIX char *aoc_getarg(char *argv[], int argc);

#define ArgMissingOptarg -1
#define ArgNotOpt -2
#define ArgNotDefined -3
#define ArgNoOptAvailable -4
/*
 * Argument parser
*/
AOCLIBS_PREFIX int aoc_getopt(char *argv[], int argc, const CLIProgram *opts);

static inline void bashgen_shebang(void) {
        puts("#!/usr/bin/env bash");
}

static inline void bashgen_env(const CLIEnv *env, size_t envc) {
        for (size_t i = 0; i < envc; i++)
                printf("%s=%s\n", env[i].name, env[i].value);
}

static inline void bashgen_argument(const CLIArgument *args) {
        char ARG[AOC_CLI_BUFFER];
        if (args && args->completion)
                aoc_normalize_shell_name(ARG, args->name, AOC_CLI_BUFFER);
        else
                return;
        printf("_%s() {\n  %s\n}\n", ARG, args->completion);
}

static inline void bashgen_flags(const CLIProgram *prog) {
        putchar(' ');
        for (size_t i = 0; i < prog->flagc; i++) {
                if (prog->flags[i].long_opt != NULL) printf(" %s", prog->flags[i].long_opt);
                if (prog->flags[i].short_opt != NULL) printf(" %s", prog->flags[i].short_opt);
        }
}

static inline void bashgen_commands(const CLIProgram *prog) {
        for (size_t i = 0; i < prog->cmdc; i++)
                printf(" %s", prog->commands[i].cmd);
}

static inline void bashgen_flag_cases(const CLIProgram *prog, const CLIOption *flags) {
        char ARG[AOC_CLI_BUFFER];
        if (flags->args && flags->args->completion)
                aoc_normalize_shell_name(ARG, prog->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *SHORT_FLAG = flags->short_opt;
        const char *LONG_FLAG = flags->long_opt;
        if (LONG_FLAG && SHORT_FLAG)
                printf("  %s|%s)\n", SHORT_FLAG, LONG_FLAG);
        else if (LONG_FLAG)
                printf("  %s)\n", LONG_FLAG);
        else if (SHORT_FLAG)
                printf("  %s)\n", SHORT_FLAG);
        else
                return;
        printf("    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n", ARG);
        printf("    return 0\n    ;;\n");
}

static inline void bashgen_command_cases(const CLIProgram *prog, const CLICommand *commands) {
        char ARG[AOC_CLI_BUFFER];
        if (commands->args && commands->args->completion)
                aoc_normalize_shell_name(ARG, prog->args->name, AOC_CLI_BUFFER);
        else
                return;
        printf("  %s)\n", commands->cmd);
        printf("    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n", ARG);
        printf("    return 0\n    ;;\n");
}

AOCLIBS_PREFIX void aoc_bashgen(const CLIProgram prog, const CLIEnv *env, size_t envc) {
        bashgen_shebang();
        bashgen_env(env, envc);
        for (size_t i = 0; i < prog.argc; i++) {
                bashgen_argument(&prog.args[i]);
        }

        // Main function
        printf("_%s() {\n", prog.name);
        printf("  local cur prev\n");
        printf("  cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");
        printf("  prev=\"${COMP_WORDS[COMP_CWORD-1]}\"\n");
        printf("  COMPREPLY=()\n");

        // Flag completion
        printf("  if [[ \"${cur}\" == -* ]]; then\n");
        printf("    mapfile -t COMPREPLY < <(compgen -W \"");
        bashgen_flags(&prog);
        printf("\" -- \"${cur}\")\n");
        printf("    return 0\n");
        printf("  fi\n");

        // Argument completion
        printf("  case \"${prev}\" in\n");
        for (size_t i = 0; i < prog.cmdc; i++) {
                bashgen_command_cases(&prog, &prog.commands[i]);
        }
        for (size_t i = 0; i < prog.flagc; i++) {
                bashgen_flag_cases(&prog, &prog.flags[i]);
        }
        printf("  esac\n");

        // Command completion
        printf("  mapfile -t COMPREPLY < <(compgen -W \"");
        bashgen_commands(&prog);
        printf("\" -- \"${cur}\")\n");
        printf("  return 0\n");
        printf("}\n");

        // Assign function to program
        printf("complete -F _%s %s\n", prog.name, prog.name);
}

AOCLIBS_PREFIX void aoc_normalize_shell_name(char *buff, const char *str, size_t buff_size) {
        if (!buff || !str || buff_size == 0) {
                if (buff && buff_size > 0) buff[0] = '\0';
                return;
        }

        size_t i = 0;

        for (; *str && i < buff_size; str++) {
                if (isalnum(*str) || *str == '_') {
                        buff[i++] = *str;
                } else {
                        buff[i++] = '_';
                }
        }

        if (i < buff_size)
                buff[i] = '\0';
        else
                buff[buff_size - 1] = '\0';

        if (i > 0 && isdigit(buff[0])) {
                memmove(buff + 1, buff, i);
                buff[0] = '_';
        }
}

// Printh

/*
 * Helper to print headings
*/
static inline void print_header(const char *msg, const char *style) {
        fprintf(stderr, "%s%s%s", style, msg, COLOR_RESET);
}

/*
 * Helper qsort to sort commands
*/
static inline int qsort_compare_cmd(const void *a, const void *b) {
        const CLICommand *CMD_A = (const CLICommand *)a;
        const CLICommand *CMD_B = (const CLICommand *)b;

        char fullA[AOC_CLI_BUFFER], fullB[AOC_CLI_BUFFER];
        snprintf(fullA, sizeof(fullA), "%s", CMD_A->cmd);
        snprintf(fullB, sizeof(fullB), "%s", CMD_B->cmd);

        return strcmp(fullA, fullB);
}

static inline const char *qsort_get_opt(const CLIOption *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

/*
 * Helper qsort to sort options
*/
static inline int qsort_compare_opts(const void *a, const void *b) {
        const CLIOption *FLAG_A = (const CLIOption *)a;
        const CLIOption *FLAG_B = (const CLIOption *)b;

        return strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

static inline bool has_commands(CLIProgram info) {
        for (size_t i = 0; i < info.cmdc; i++) {
                if (info.commands->cmd != NULL) return true;
        }
        return false;
}

static inline bool has_args(CLIProgram info) {
        for (size_t i = 0; i < info.cmdc; i++) {
                if (info.args->name != NULL) return true;
        }
        return false;
}

static inline bool has_options(CLIProgram info) {
        for (size_t i = 0; i < info.flagc; i++) {
                if (info.flags->long_opt != NULL || info.flags->short_opt != NULL) return true;
        }
        return false;
}

// Arguments:
//   arg1
//       Description
//   arg2
//       Description
static inline void printh_arguments(CLIProgram *prog) {
        print_header("Arguments:\n", COLOR_BOLD_UNDERLINE);
        for (size_t i = 0; i < prog->argc; i++) {
                const char *ARG_NAME = prog->args[i].name;
                const char *ARG_DESC = prog->args[i].desc;

                char arg_full[AOC_CLI_BUFFER] = { 0 };

                if (ARG_NAME) {
                        snprintf(arg_full, sizeof(arg_full), "%s%s%s", COLOR_BOLD, ARG_NAME,
                                 COLOR_RESET);
                        fprintf(stderr, "  %s\n", arg_full);
                        if (ARG_DESC && strlen(ARG_DESC) > 0) {
                                aoc_iprint(ARG_DESC, AOC_PRINTH_DESC_INDENT);
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
static inline void printh_commands(CLIProgram *prog) {
        print_header("Commands:\n", COLOR_BOLD_UNDERLINE);

        for (size_t i = 0; i < prog->cmdc; i++) {
                const char *CMD = prog->commands[i].cmd;
                const char *ARG = (prog->commands[i].args) ? prog->commands[i].args->name : NULL;
                const char *DESC = prog->commands[i].desc;

                char cmd_full[AOC_CLI_BUFFER] = { 0 };

                if (ARG) {
                        snprintf(cmd_full, sizeof(cmd_full), "%s%s%s [%s]", COLOR_BOLD, CMD,
                                 COLOR_RESET, ARG);
                } else {
                        snprintf(cmd_full, sizeof(cmd_full), "%s%s%s", COLOR_BOLD, CMD,
                                 COLOR_RESET);
                }

                fprintf(stderr, "  %s\n", cmd_full);
                if (DESC && strlen(DESC) > 0) {
                        aoc_iprint(DESC, AOC_PRINTH_DESC_INDENT);
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
static inline void printh_options(CLIProgram *prog) {
        print_header("Options:\n", COLOR_BOLD_UNDERLINE);

        for (size_t i = 0; i < prog->flagc; i++) {
                const char *SHORT_OPT = prog->flags[i].short_opt;
                const char *LONG_OPT = prog->flags[i].long_opt;
                const char *ARG = (prog->flags[i].args) ? prog->flags[i].args->name : NULL;
                const char *DESC = prog->flags[i].desc;

                char flag_buffer[AOC_CLI_BUFFER] = { 0 };

                if (SHORT_OPT && LONG_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s, %s%s%s", COLOR_BOLD,
                                 SHORT_OPT, COLOR_RESET, COLOR_BOLD, LONG_OPT, COLOR_RESET);
                } else if (LONG_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s", COLOR_BOLD, LONG_OPT,
                                 COLOR_RESET);
                } else if (SHORT_OPT) {
                        snprintf(flag_buffer, sizeof(flag_buffer), "%s%s%s", COLOR_BOLD, SHORT_OPT,
                                 COLOR_RESET);
                }

                if (ARG) {
                        strcat(flag_buffer, " [");
                        strcat(flag_buffer, ARG);
                        strcat(flag_buffer, "]");
                        break;
                }

                fprintf(stderr, "  %s\n", flag_buffer);
                if (DESC && strlen(DESC) > 0) {
                        aoc_iprint(DESC, AOC_PRINTH_DESC_INDENT);
                        fputc('\n', stderr);
                }
        }
}

AOCLIBS_PREFIX void aoc_printh(CLIProgram prog) {
        qsort(prog.commands, prog.cmdc, sizeof(CLICommand), qsort_compare_cmd);
        qsort(prog.flags, prog.flagc, sizeof(CLIOption), qsort_compare_opts);

        // program name | program description
        fprintf(stderr, "%s | %s\n\n", prog.name, prog.desc);

        // Usage: program usage
        print_header("Usage:", COLOR_BOLD_UNDERLINE);
        fprintf(stderr, "  %s%s%s %s\n\n", COLOR_BOLD, prog.name, COLOR_RESET, prog.usage);

        if (has_args(prog)) printh_arguments(&prog);
        if (has_commands(prog)) printh_commands(&prog);
        if (has_options(prog)) printh_options(&prog);
}

// Zshgen

static inline void zshgen_print_arg_autocomplete(const CLIProgram *prog, const CLIArgument *args) {
        char ARG[AOC_CLI_BUFFER] = { 0 };
        if (args)
                aoc_normalize_shell_name(ARG, args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *COMPLETIONS = args->completion;
        if (COMPLETIONS != NULL) {
                printf("_%s_get_%s() {\n", prog->name, ARG);
                printf("  local results\n");
                printf("  results=(${(f)\"$(%s 2>/dev/null)\"})\n", COMPLETIONS);
                printf("  compadd -Q -a results\n");
                printf("}\n\n");
        }
}

static void zshgen_print_flag_arg(const CLIProgram *prog, const CLIOption *flag) {
        char ARG[AOC_CLI_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                aoc_normalize_shell_name(ARG, flag->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *DESC = flag->desc;
        const char *COMP = (flag->args) ? flag->args->completion : NULL;

        if (!COMP) return;

        if (LONG_FLAG) {
                printf("    '%s", LONG_FLAG);
                if (DESC) printf("=[%s]", DESC);
                if (COMP) {
                        printf(":%s:_%s_get_%s", ARG, prog->name, ARG);
                } else {
                        printf(":%s", ARG);
                }
                printf("' \\\n");
        }
        if (SHORT_FLAG) {
                printf("    '%s", SHORT_FLAG);
                if (DESC) printf("[%s]", DESC);
                if (COMP) {
                        printf(":%s:_%s_get_%s", ARG, prog->name, ARG);
                } else {
                        printf(":%s", ARG);
                }
                printf("' \\\n");
        }
}

static inline void zshgen_print_command_case(const CLIProgram *prog, const CLICommand *cmd) {
        char ARG[AOC_CLI_BUFFER];
        if (cmd->args)
                aoc_normalize_shell_name(ARG, cmd->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *COMP = (cmd->args) ? cmd->args->completion : NULL;
        if (!COMP) return;

        printf("        %s)\n", cmd->cmd);
        printf("          _arguments \\\n");
        printf("            '*:%s:_%s_get_%s' \\\n", ARG, prog->name, ARG);
        printf("          ;;\n");
}

static inline void zshgen_print_flag_case(const CLIProgram *prog, const CLIOption *flag) {
        char ARG[AOC_CLI_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                aoc_normalize_shell_name(ARG, flag->args->name, AOC_CLI_BUFFER);
        else
                return;
        const char *COMPLETIONS = (flag->args) ? flag->args->completion : NULL;
        if (!COMPLETIONS) return;

        if (SHORT_FLAG != NULL && LONG_FLAG != NULL) {
                printf("        %s | %s)\n", SHORT_FLAG, LONG_FLAG);
        } else if (SHORT_FLAG != NULL) {
                printf("        %s)\n", SHORT_FLAG);
        } else if (LONG_FLAG != NULL) {
                printf("        %s)\n", LONG_FLAG);
        }
        printf("          _arguments \\\n");
        printf("            '*:%s:_%s_get_%s' \\\n", ARG, prog->name, ARG);
        printf("          return\n");
        printf("          ;;\n");
}

AOCLIBS_PREFIX void zshgen(const CLIProgram prog, const CLIEnv *env, size_t envc) {
        // Header
        printf("#compdef %s\n\n", prog.name);

        // Environment defaults
        for (size_t i = 0; i < envc; i++) {
                printf("%s=%s\n", env[i].name, env[i].value);
        }

        // Main function
        printf("_%s() {\n", prog.name);
        printf("  local -a subcommands\n\n");

        // Define Subcommands
        printf("  subcommands=(\n");
        for (size_t i = 0; i < prog.cmdc; i++) {
                printf("    \"%s:%s\"\n", prog.commands[i].cmd, prog.commands[i].desc);
        }
        printf("  )\n\n");

        // Define arguments
        printf("  _arguments -C \\\n");
        printf("    '1:command:->subcmds' \\\n");
        for (size_t i = 0; i < prog.flagc; i++) {
                zshgen_print_flag_arg(&prog, &prog.flags[i]);
        }
        printf("    '*::args:->command_args'\n\n");

        // Autocompletion
        printf("  case $state in\n");
        printf("    subcmds)\n");
        printf("      _describe 'command' subcommands\n");
        printf("      return\n");
        printf("      ;;\n");
        printf("    command_args)\n");
        printf("      case $words[1] in\n");

        // Autocomplete arguments from commands
        for (size_t i = 0; i < prog.cmdc; i++) {
                zshgen_print_command_case(&prog, &prog.commands[i]);
        }
        // Autocomplete arguments from flags
        for (size_t i = 0; i < prog.flagc; i++) {
                zshgen_print_flag_case(&prog, &prog.flags[i]);
        }
        printf("      esac\n");
        printf("      ;;\n");
        printf("  esac\n");
        printf("}\n\n");

        // Define helper functions to autocomplete arguments
        for (size_t i = 0; i < prog.argc; i++) {
                zshgen_print_arg_autocomplete(&prog, &prog.args[i]);
        }

        // Assign function to program
        printf("compdef _%s %s\n", prog.name, prog.name);
}

// CLI Argument Parser

AOCLIBS_PREFIX char *aoc_getarg(char *argv[], int argc) {
        if (optind >= argc) return NULL;
        return argv[optind++];
}

AOCLIBS_PREFIX int aoc_getopt(char *argv[], int argc, const CLIProgram *opts) {
        const char *arg = aoc_getarg(argv, argc);
        optcur = (char *)arg;
        if (!arg || arg[0] != '-') return ArgNotOpt;

        const CLIOption *opt = opts->flags;
        if (opt == NULL) return ArgNoOptAvailable;

        for (size_t i = 0; i < opts->flagc; i++) {
                const char *long_opt = opt[i].long_opt;
                const char *short_opt = opt[i].short_opt;
                const CLIArgument *flag_arg = opt[i].args;

                if (long_opt != NULL && cstr_eq(arg, long_opt)) {
                        if (flag_arg != NULL) {
                                optarg = aoc_getarg(argv, argc);
                                if (optarg == NULL) return ArgMissingOptarg;
                                if (optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }

                if (short_opt != NULL && cstr_eq(arg, short_opt)) {
                        if (flag_arg != NULL) {
                                optarg = aoc_getarg(argv, argc);
                                if (optarg == NULL) return ArgMissingOptarg;
                                if (optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }
        }
        return ArgNotDefined;
}

AOCLIBS_PREFIX void aoc_iprint(const char *msg, int indent) {
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
                w.ws_col = 80;
        }
        const int WIDTH = w.ws_col;

        fprintf(stderr, "%-*s", indent, "");
        int line_pos = indent;

        const char *START = msg;
        const char *END = msg;

        while (*END) {
                if (*END == '\n') {
                        fputc('\n', stderr);
                        fprintf(stderr, "%*s", indent, "");
                        line_pos = indent;
                        END++;
                        continue;
                }
                while (isspace((unsigned char)*END))
                        END++;

                if (*END == ' ') break;

                START = END;
                while (*END && !isspace((unsigned char)*END))
                        END++;

                int word_len = END - START;

                if (line_pos + word_len > WIDTH && line_pos > indent) {
                        fprintf(stderr, "\n%*s", indent, "");
                        line_pos = indent;
                }

                fprintf(stderr, "%.*s", word_len, START);
                line_pos += word_len;

                if (*END && line_pos < WIDTH) {
                        fputc(' ', stderr);
                        line_pos++;
                }
        }
        fputc(' ', stderr);
}

#endif // AOCLIBS_CLI_H_
