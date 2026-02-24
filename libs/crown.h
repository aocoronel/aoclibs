#ifndef AOCLIBS_CROWN_H_
#define AOCLIBS_CROWN_H_

#include "arena.h"
#include "base.h"
#include <assert.h>
#include <alloca.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CROWN_BUFFER 256 // used by crown_normalize_name
#define CROWN_INDENTATION 10 // Indentation by spaces

// Coloring
#define CROWN_HEADER_COLOR COLOR_BOLD_UNDERLINE
#define CROWN_COMMAND_COLOR COLOR_BOLD

#ifndef CROWN_OUTPUT
#define CROWN_OUTPUT stdout
#endif

int optind = 0; // == getopt
char *optarg = NULL; // == getopt
char *optcur = NULL; // opt current

typedef struct CrownCommand CrownCommand;

typedef struct {
        const char *name;
        const char *completion;
} CrownArgument;

typedef struct {
        const char *short_opt;
        const char *long_opt;
        size_t args;
        const char *desc;
} CrownOption;

typedef struct {
        size_t len;
        size_t cap;
        CrownOption *data;
} CrownOpts;

typedef struct {
        size_t len;
        size_t cap;
        CrownArgument *data;
} CrownArgs;

typedef struct {
        size_t len;
        size_t cap;
        CrownCommand *data;
} CrownCmds;

struct CrownCommand {
        const char *name;
        size_t args;
        const char *desc;
        CrownCmds *subcmd;
        CrownOpts *flags;
};

typedef struct {
        CrownOption *opt;
        const char *name;
        const char *value; // HOME, PATH, SHELL...
} CrownEnv;

typedef struct {
        const char *name;
        const char *desc;
        const char *usage;
        CrownArgs *args;
        CrownCmds *subcmd;
        CrownOpts *flags;
        const char *version;
} CrownProgram;

CrownCmds Crown_Command = { 0 };
CrownOpts Crown_Option = { 0 };
CrownArgs Crown_Argument = { 0 };

CrownProgram *Program;
Arena Program_Arena = { 0 };

static CrownCommand *last_cmd = NULL;

#define CROWN_PRINTF(...) fprintf(CROWN_OUTPUT, __VA_ARGS__)

// Important to initialize, before anything at the beginning of the main()
// You may set .name, .usage and .desc here.
#define crown_init(...)                                                \
        do {                                                           \
                CrownProgram *_Program = alloca(sizeof(CrownProgram)); \
                *_Program = (CrownProgram){ .subcmd = &Crown_Command,  \
                                            .flags = &Crown_Option,    \
                                            .args = &Crown_Argument,   \
                                            __VA_ARGS__ };             \
                Program = _Program;                                    \
                Program->args->len = 1;                                \
        } while (0)

// Once you no longer need to use Crown, you can deinitialize all memory it used.
AOCLIBS_PREFIX void crown_deinit(void);

// Add new argument
// It has a key which is used to be referenced by flags and commands
#define crown_new_arg(key, ...)                \
        size_t key##_arg = Program->args->len; \
        crown_append(CrownArgument, Program->args, (CrownArgument){ __VA_ARGS__ })

#define crown_new_opt(key, opt, ...)          \
        size_t key##_opt = (opt)->flags->len; \
        crown_append(CrownOption, (opt)->flags, (CrownOption){ __VA_ARGS__ })

// crown_new_cmd(open, Program, ...)
#define crown_new_cmd(key, opt, ...)                                                      \
        size_t key##_cmd = (opt)->subcmd->len;                                            \
        do {                                                                              \
                crown_append(CrownCommand, (opt)->subcmd, (CrownCommand){ __VA_ARGS__ }); \
                last_cmd = &(opt)->subcmd->data[(opt)->subcmd->len - 1];                  \
                last_cmd->subcmd = aoc_arena_calloc(&Program_Arena, sizeof(CrownCmds));   \
                last_cmd->flags = aoc_arena_calloc(&Program_Arena, sizeof(CrownOpts));    \
        } while (0);                                                                      \
        CrownCommand *key = &(opt)->subcmd->data[key##_cmd];

// Syntactic sugar for crown_new_opt(last_cmd->flags, ...)
#define crown_new_subopt(key, ...)                                                        \
        size_t key##_opt = last_cmd->flags->len;                                          \
        do {                                                                              \
                assert(last_cmd != NULL);                                                 \
                crown_append(CrownOption, last_cmd->flags, (CrownOption){ __VA_ARGS__ }); \
        } while (0)

#define crown_new_subcmd(key, ...)                                                           \
        size_t key##_cmd = last_cmd->subcmd->len;                                            \
        do {                                                                                 \
                assert(last_cmd != NULL);                                                    \
                crown_append(CrownCommand, last_cmd->subcmd, (CrownCommand){ __VA_ARGS__ }); \
                last_cmd->subcmd->data[key##_cmd].subcmd =                                   \
                        aoc_arena_calloc(&Program_Arena, sizeof(CrownCmds));                 \
                last_cmd->subcmd->data[key##_cmd].flags =                                    \
                        aoc_arena_calloc(&Program_Arena, sizeof(CrownOpts));                 \
        } while (0);                                                                         \
        CrownCommand *key = &last_cmd->subcmd->data[key##_cmd];

// Internal macro
#define crown_append(T, opt, ...)                                     \
        do {                                                          \
                T _tmp = __VA_ARGS__;                                 \
                void *ptr = &_tmp;                                    \
                aoc_dar_reserve(&Program_Arena, opt, (opt)->len + 1); \
                memcpy(&(opt)->data[(opt)->len], ptr, sizeof(T));     \
                (opt)->len += 1;                                      \
        } while (0)

// Prints indented message of given "indent".
//
// If the size of the terminal cannot be obtained, fallback to 80 columns.
AOCLIBS_PREFIX void crown_iprint(const char *msg, int indent);

// Generates bash completions for commands, flags and arguments.
//
// If user provides environment variables, they will be set as a global completion variable.
// This way the user can write argument completion that uses those environment variables.
AOCLIBS_PREFIX void crown_bashgen(const CrownEnv *null env, size_t envc);

// Normalizes a given "str" to a valid shell function name, by replacing it to non-alphanumeric
// characters with underscores.
//
// May truncate to "buff_size".
AOCLIBS_PREFIX void crown_normalize_name(char *buff, const char *str, size_t buff_size);

// Prints help message.
//
// If "cmd" is NULL, prints the main help message.
AOCLIBS_PREFIX void crown_help(CrownCommand *null cmd);

// Get next argument from argv.
AOCLIBS_PREFIX char *crown_getarg(char *argv[], int argc);

// Important errors when using getopt:

enum {
        // Returned when CrownOption.args is not NULL, and no argument has been found.
        ArgMissingOptarg = -1,
        // Returned when input is not recognized to be a flag.
        // This can be used to parse commands and positional arguments.
        ArgNotOpt,
        // This is relevant only for developers, and notices when there was an error
        // on defining and option or command.
        ArgNotDefined,
        // This should NEVER return. It only returns if optind is higher than argc.
        EndOfArgs,
        // User attempted to enter an option that doesn't exist.
        ArgNotFound,
};

#define crown_parseopt(opt) crown_getopt((opt), argv, argc)
// Flag parser.
//
// If "cmds" is NULL, fallsback to flags defined in Programs.
//
// By providing a specific command, getopt will return the index, related to the options defined in
// the provided command. If "cmds" is NULL, parses the main commands.
//
// The returned value is the given index generated when creating a command, argument or option with
// crown_new_cmd...
AOCLIBS_PREFIX int crown_getopt(CrownCommand *null cmds, char *argv[], int argc);

// Convenient way to access a subcmd
#define crown_subcmd(opt, idx) (opt)->subcmd->data[(idx)]

#define crown_parsecmd(opt) crown_getcmd((opt), argv, argc)
// Command parser.
//
// If "cmds" is NULL, fallsback to commands defined in Programs.
//
// By providing a specific command, getcmd will return the index, related to the subcommand defined
// in the provided command. If "cmds" is NULL, parses the main commands.
//
// The returned value is the given index generated when creating a command, argument or option with
// crown_new_cmd...
AOCLIBS_PREFIX int crown_getcmd(CrownCommand *null cmds, char *argv[], int argc);

#ifdef AOCLIBS_CROWN

#include "colors.h"
#include "cstr.h"
#include <ctype.h>

AOCLIBS_PREFIX void crown_deinit(void) {
        aoc_arena_destroy(&Program_Arena);
}

// Helper function
AOCLIBS_PREFIX void crown_indent_completion(int indent) {
        for (int i = 0; i < indent; i++) {
                fputc(' ', CROWN_OUTPUT);
        }
}

// Helper function
AOCLIBS_PREFIX void crown_bashgen_case_prev_open(void) {
        CROWN_PRINTF("  case \"${prev}\" in\n");
}

// Helper function
AOCLIBS_PREFIX void crown_bashgen_case_prev_close(int indent) {
        crown_indent_completion(indent);
        CROWN_PRINTF("  esac\n");
}

// Helper function
AOCLIBS_PREFIX void crown_bashgen_options(CrownOpts *cmds, int indent) {
        CrownOpts *curr_cmd = cmds == NULL ? Program->flags : cmds;
        ASSERT(curr_cmd != NULL);

        foreach (curr_cmd, i) {
                char ARG[CROWN_BUFFER];
                CrownOption flags = curr_cmd->data[i];
                CrownArgument arg = Program->args->data[flags.args];
                if (arg.name) crown_normalize_name(ARG, arg.name, CROWN_BUFFER);
                const char *SHORT_FLAG = flags.short_opt;
                const char *LONG_FLAG = flags.long_opt;

                ASSERT(LONG_FLAG || SHORT_FLAG, "Option doesn't has a short or long flag");

                crown_indent_completion(indent);
                if (LONG_FLAG && SHORT_FLAG)
                        CROWN_PRINTF("  %s|%s)\n", SHORT_FLAG, LONG_FLAG);
                else if (LONG_FLAG)
                        CROWN_PRINTF("  %s)\n", LONG_FLAG);
                else if (SHORT_FLAG)
                        CROWN_PRINTF("  %s)\n", SHORT_FLAG);

                if (arg.name) {
                        crown_indent_completion(indent);
                        CROWN_PRINTF("    COMPREPLY=(\"$(compgen -W \"$(_%s)\" -- \"${cur}\")\")\n",
                                     ARG);
                }
                crown_indent_completion(indent);
                CROWN_PRINTF("    return 0\n");
                crown_indent_completion(indent);
                CROWN_PRINTF("    ;;\n");
        }
}

// Helper function
AOCLIBS_PREFIX void crown_bashgen_subcommand(CrownCmds *cmds, int indent) {
        CrownCmds *curr_cmd = cmds == NULL ? Program->subcmd : cmds;
        ASSERT(curr_cmd != NULL);

        foreach (curr_cmd, i) {
                char ARG[CROWN_BUFFER];
                CrownCommand cmd = curr_cmd->data[i];
                CrownArgument arg = Program->args->data[cmd.args];
                if (arg.name) crown_normalize_name(ARG, arg.name, CROWN_BUFFER);

                crown_indent_completion(indent);
                CROWN_PRINTF("  %s)\n", cmd.name);

                if (cmd.subcmd && cmd.subcmd->data) {
                        crown_indent_completion(indent + 2);
                        crown_bashgen_case_prev_open();
                        foreach (cmd.subcmd, j) {
                                crown_bashgen_subcommand(cmd.subcmd, indent + 4);
                        }
                        crown_bashgen_case_prev_close(indent + 2);
                }

                if (cmd.flags && cmd.flags->data) {
                        crown_indent_completion(indent + 2);
                        crown_bashgen_case_prev_open();
                        foreach (cmd.flags, j) {
                                crown_bashgen_options(cmd.flags, indent + 4);
                        }
                        crown_bashgen_case_prev_close(indent + 2);
                }

                if (arg.name) {
                        crown_indent_completion(indent);
                        CROWN_PRINTF("    COMPREPLY=(\"$(compgen -W \"$(_%s)\" -- \"${cur}\")\")\n",
                                     ARG);
                        if (cmd.subcmd != NULL) {
                                eprintf("%s[WARNING]%s The command %s has subcommands and an argument. Crown expects to be either one or the other\n",
                                        COLOR_YELLOW,
                                        COLOR_RESET,
                                        cmd.name);
                        }
                } else {
                        CROWN_PRINTF("    COMPREPLY=(\"$(compgen -W \"");
                        foreach (cmd.subcmd, j) {
                                CROWN_PRINTF("%s ", cmd.subcmd->data[j].name);
                        }
                        foreach (cmd.flags, j) {
                                CrownOption _tmp = cmd.flags->data[j];
                                if (_tmp.short_opt) CROWN_PRINTF("%s ", _tmp.short_opt);
                                if (_tmp.long_opt) CROWN_PRINTF("%s ", _tmp.long_opt);
                        }
                        CROWN_PRINTF("\" -- \"${cur}\")\")\n");
                }

                if (cmd.subcmd == NULL) indent -= 2;
                crown_indent_completion(indent);
                CROWN_PRINTF("    return 0\n");
                crown_indent_completion(indent);
                CROWN_PRINTF("    ;;\n");
        }
}

AOCLIBS_PREFIX void crown_bashgen_env_vars(const CrownEnv *env, size_t envc) {
        if (env == NULL || envc == 0) return;
        for (size_t j = 0; j < envc; j++) {
                ASSERT(envc <= Program->args->len);
                ASSERT(Program->args->data[env->opt->args].name != NULL);
                if (!aoc_cstr_eq(Program->args->data[env->opt->args].name, env->name)) continue;
                CROWN_PRINTF("  for ((i = 0; i < ${#COMP_WORDS[@]}; i++)); do\n");
                if (env[j].opt->short_opt && env[j].opt->long_opt) {
                        CROWN_PRINTF(
                                "    if [[ \"${COMP_WORDS[i]}\" == \"%s\" ]] || [[ \"${COMP_WORDS[i]}\" == \"%s\" ]] && ((i + 1 < ${#COMP_WORDS[@]})); then\n",
                                env[j].opt->short_opt,
                                env[j].opt->long_opt);
                } else if (env[j].opt->short_opt) {
                        CROWN_PRINTF(
                                "    if [[ \"${COMP_WORDS[i]}\" == \"%s\" ]] && ((i + 1 < ${#COMP_WORDS[@]})); then\n",
                                env[j].opt->long_opt);
                } else if (env[j].opt->long_opt) {
                        CROWN_PRINTF(
                                "    if [[ \"${COMP_WORDS[i]}\" == \"%s\" ]] && ((i + 1 < ${#COMP_WORDS[@]})); then\n",
                                env[j].opt->short_opt);
                }
                CROWN_PRINTF("       %s=\"${COMP_WORDS[i + 1]}\"\n", env->name);
                CROWN_PRINTF("       break\n");
                CROWN_PRINTF("    fi\n");
                CROWN_PRINTF("  done\n");
        }
}

AOCLIBS_PREFIX void crown_bashgen(const CrownEnv *env, size_t envc) {
        CROWN_PRINTF("#!/usr/bin/env bash");

        // Sets all environment variables to the top
        for (size_t i = 0; i < envc; i++)
                CROWN_PRINTF("%s=%s\n", env[i].name, env[i].value);

        // Generates all the functions responsible for the completions
        //
        // Completes using ls:
        // args.completion = "ls"
        for (size_t i = 0; i < Program->args->len; i++) {
                CrownArgument args = Program->args->data[i];
                char ARG[CROWN_BUFFER];
                if (args.completion)
                        crown_normalize_name(ARG, args.name, CROWN_BUFFER);
                else
                        continue;
                CROWN_PRINTF("_%s() {\n  %s\n}\n", ARG, args.completion);
        }

        // Main function
        CROWN_PRINTF("_%s() {\n", Program->name);
        CROWN_PRINTF("  local cur prev\n");
        CROWN_PRINTF("  cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");
        CROWN_PRINTF("  prev=\"${COMP_WORDS[COMP_CWORD-1]}\"\n");
        CROWN_PRINTF("  COMPREPLY=()\n");

        // Flag completion
        CROWN_PRINTF("  if [[ \"${cur}\" == -* ]]; then\n");
        CROWN_PRINTF("    COMPREPLY=(\"$(compgen -W \"");

        fputc(' ', CROWN_OUTPUT);
        for (size_t i = 0; i < Program->flags->len; i++) {
                CrownOption flags = Program->flags->data[i];
                if (flags.long_opt != NULL) CROWN_PRINTF(" %s", flags.long_opt);
                if (flags.short_opt != NULL) CROWN_PRINTF(" %s", flags.short_opt);
        }

        CROWN_PRINTF("\" -- \"${cur}\")\")\n");
        CROWN_PRINTF("    return 0\n");
        CROWN_PRINTF("  fi\n");

        crown_bashgen_env_vars(env, envc);

        // Argument completion
        CROWN_PRINTF("  case \"${prev}\" in\n");
        // Commands
        crown_bashgen_subcommand(NULL, 0);
        // Flags
        crown_bashgen_options(NULL, 0);
        CROWN_PRINTF("  esac\n");

        // Command completion
        CROWN_PRINTF("  COMPREPLY=(\"$(compgen -W \"");

        for (size_t i = 0; i < Program->subcmd->len; i++) {
                CrownCommand cmds = Program->subcmd->data[i];
                CROWN_PRINTF(" %s", cmds.name);
        }

        CROWN_PRINTF("\" -- \"${cur}\")\")\n");
        CROWN_PRINTF("  return 0\n");
        CROWN_PRINTF("}\n");

        // Assign function to program
        CROWN_PRINTF("complete -F _%s %s\n", Program->name, Program->name);
}

AOCLIBS_PREFIX void crown_normalize_name(char *buff, const char *str, size_t buff_size) {
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

// crown_help

// Helper function
AOCLIBS_PREFIX void crown_print_header(const char *msg) {
        CROWN_PRINTF("%s%s%s", CROWN_HEADER_COLOR, msg, COLOR_RESET);
}

// Helper function
AOCLIBS_PREFIX int crown_help_qsort_cmd(const void *a, const void *b) {
        const CrownCommand *CMD_A = (const CrownCommand *)a;
        const CrownCommand *CMD_B = (const CrownCommand *)b;

        return strcmp(CMD_A->name, CMD_B->name);
}

// Helper function
AOCLIBS_PREFIX const char *qsort_get_opt(const CrownOption *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

// Helper function
AOCLIBS_PREFIX int crown_help_qsort_opt(const void *a, const void *b) {
        const CrownOption *FLAG_A = (const CrownOption *)a;
        const CrownOption *FLAG_B = (const CrownOption *)b;

        return strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

// Helper function
AOCLIBS_PREFIX bool crown_has_commands(CrownCommand *cmds) {
        CrownCmds *cmd = cmds && cmds->subcmd != NULL ? cmds->subcmd : Program->subcmd;
        if (cmd == NULL) return false;
        if (cmd->len == 0) return false;
#ifndef NDEBUG
        ASSERT(cmd->data != NULL, "No command is defined, but length is %zu\n", cmd->len);
        for (size_t i = 0; i < cmd->len; i++)
                ASSERT(cmd->data[i].name != NULL,
                       "Found a subcommand without name in %s",
                       cmds->name);
#endif
        return true;
}

// Helper function
AOCLIBS_PREFIX bool crown_has_options(CrownCommand *cmds) {
        CrownOpts *opt = cmds && cmds->flags != NULL ? cmds->flags : Program->flags;
        if (opt == NULL) return false;
        if (opt->len == 0) return false;
#ifndef NDEBUG
        ASSERT(opt->data != NULL, "No option is defined, but length is %zu\n", opt->len);
        for (size_t i = 0; i < opt->len; i++)
                ASSERT(opt->data[i].short_opt != NULL || opt->data[i].long_opt != NULL,
                       "Found a option without a least one short/long flag in %s",
                       cmds->name);
#endif
        return true;
}

// Used in crown_help_commands and crown_help
#define crown_help_command(command)                                                               \
        do {                                                                                      \
                CrownArgument arg = Program->args->data[(command)->args];                         \
                if (arg.name) {                                                                   \
                        CROWN_PRINTF("  %s%s%s [%s]\n",                                           \
                                     CROWN_COMMAND_COLOR,                                         \
                                     (command)->name,                                             \
                                     COLOR_RESET,                                                 \
                                     arg.name);                                                   \
                } else {                                                                          \
                        CROWN_PRINTF(                                                             \
                                "  %s%s%s\n", CROWN_COMMAND_COLOR, (command)->name, COLOR_RESET); \
                }                                                                                 \
        } while (0)

#define crown_help_usage(command)                                                                \
        do {                                                                                     \
                CrownArgument arg = Program->args->data[(command)->args];                        \
                if (arg.name) {                                                                  \
                        CROWN_PRINTF(" %s%s%s [%s]\n",                                           \
                                     CROWN_COMMAND_COLOR,                                        \
                                     (command)->name,                                            \
                                     COLOR_RESET,                                                \
                                     arg.name);                                                  \
                } else {                                                                         \
                        CROWN_PRINTF(                                                            \
                                " %s%s%s\n", CROWN_COMMAND_COLOR, (command)->name, COLOR_RESET); \
                }                                                                                \
        } while (0)

// Commands:
//   cmd2 [ARG]
//       Description

// Helper function
AOCLIBS_PREFIX void crown_help_commands(CrownCommand *cmds) {
        crown_print_header("Commands:\n");

        const CrownCmds *COMMAND = cmds && cmds->subcmd != NULL ? cmds->subcmd : Program->subcmd;

        for (size_t i = 0; i < COMMAND->len; i++) {
                if (COMMAND->data == NULL) continue;
                CrownCommand cmd = COMMAND->data[i];

                crown_help_command(&cmd);

                if (cmd.desc && cmd.desc[0] != '\0') {
                        crown_iprint(cmd.desc, CROWN_INDENTATION);
                        fputc('\n', CROWN_OUTPUT);
                }
        }
        fputc('\n', CROWN_OUTPUT);
}

#define crown_help_option(opt)                                                                 \
        do {                                                                                   \
                const char *SHORT_OPT = (opt)->short_opt;                                      \
                const char *LONG_OPT = (opt)->long_opt;                                        \
                CrownArgument arg = Program->args->data[(opt)->args];                          \
                if (SHORT_OPT && LONG_OPT) {                                                   \
                        CROWN_PRINTF("  %s%s%s, %s%s%s",                                       \
                                     CROWN_COMMAND_COLOR,                                      \
                                     SHORT_OPT,                                                \
                                     COLOR_RESET,                                              \
                                     CROWN_COMMAND_COLOR,                                      \
                                     LONG_OPT,                                                 \
                                     COLOR_RESET);                                             \
                } else if (LONG_OPT) {                                                         \
                        CROWN_PRINTF("  %s%s%s", CROWN_COMMAND_COLOR, LONG_OPT, COLOR_RESET);  \
                } else if (SHORT_OPT) {                                                        \
                        CROWN_PRINTF("  %s%s%s", CROWN_COMMAND_COLOR, SHORT_OPT, COLOR_RESET); \
                } else {                                                                       \
                        continue;                                                              \
                }                                                                              \
                if (arg.name) CROWN_PRINTF(" [%s]", arg.name);                                 \
        } while (0)

// Options:
//   -s, --short [ARG]
//   -l, --long [ARG]
//   -h, --help
//       Description

// Helper function
AOCLIBS_PREFIX void crown_help_options(CrownCommand *cmds) {
        crown_print_header("Options:\n");

        const CrownOpts *FLAG = cmds && cmds->flags != NULL ? cmds->flags : Program->flags;

        for (size_t i = 0; i < FLAG->len; i++) {
                if (FLAG->data == NULL) continue;
                CrownOption flags = FLAG->data[i];

                crown_help_option(&flags);

                fputc('\n', CROWN_OUTPUT);
                if (flags.desc && flags.desc[0] != '\0') {
                        crown_iprint(flags.desc, CROWN_INDENTATION);
                        fputc('\n', CROWN_OUTPUT);
                }
        }
        fputc('\n', CROWN_OUTPUT);
}

AOCLIBS_PREFIX void crown_help(CrownCommand *null cmd) {
        // NULL in case you want to print the general flags and commands

        CrownCommand *print = cmd == NULL ? NULL : cmd;
        if (cmd == NULL) {
                qsort(Program->subcmd->data,
                      Program->subcmd->len,
                      sizeof(CrownCommand),
                      crown_help_qsort_cmd);
                qsort(Program->flags->data,
                      Program->flags->len,
                      sizeof(CrownOption),
                      crown_help_qsort_opt);

                // program name | program description
                CROWN_PRINTF("%s | %s\n\n", Program->name, Program->desc);

                // Usage: program usage
                crown_print_header("Usage:");
                printf(" %s%s%s %s\n\n",
                       CROWN_COMMAND_COLOR,
                       Program->name,
                       COLOR_RESET,
                       Program->usage);
        } else {
                qsort(cmd->subcmd->data,
                      cmd->subcmd->len,
                      sizeof(CrownCommand),
                      crown_help_qsort_cmd);
                qsort(cmd->flags->data, cmd->flags->len, sizeof(CrownOption), crown_help_qsort_opt);

                // command name | command description
                CROWN_PRINTF("%s\n\n", cmd->desc);

                crown_print_header("Usage:");
                crown_help_usage(cmd);
                fputc('\n', CROWN_OUTPUT);
        }
        if (crown_has_commands(print)) crown_help_commands(print);
        if (crown_has_options(print)) crown_help_options(print);
}

// CLI Argument Parser

AOCLIBS_PREFIX char *crown_getarg(char *argv[], int argc) {
        if (optind >= argc) return NULL;
        return argv[optind++];
}

AOCLIBS_PREFIX int crown_getopt(CrownCommand *null cmds, char *argv[], int argc) {
        const char *arg = crown_getarg(argv, argc);
        optcur = (char *)arg;

        if (!arg) return EndOfArgs;
        if (arg[0] != '-') return ArgNotOpt;

        const CrownOption *opt = cmds && cmds->flags != NULL ? cmds->flags->data :
                                                               Program->flags->data;
        const size_t len = cmds && cmds->flags != NULL ? cmds->flags->len : Program->flags->len;

        if (opt == NULL) return ArgNotDefined;

        for (size_t i = 0; i < len; i++) {
                const char *long_opt = opt[i].long_opt;
                const char *short_opt = opt[i].short_opt;
                const size_t flag_arg_idx = opt[i].args;
                const char *flag_arg = Program->args->data[i].name;

                if (long_opt != NULL && aoc_cstr_eq(arg, long_opt)) {
                        if (flag_arg != NULL) {
                                optarg = crown_getarg(argv, argc);
                                if (optarg == NULL || optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }

                if (short_opt != NULL && aoc_cstr_eq(arg, short_opt)) {
                        if (flag_arg != NULL) {
                                optarg = crown_getarg(argv, argc);
                                if (optarg == NULL || optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }
        }
        return ArgNotFound;
}

AOCLIBS_PREFIX int crown_getcmd(CrownCommand *null cmds, char *argv[], int argc) {
        const CrownCommand *opt = cmds && cmds->subcmd != NULL ? cmds->subcmd->data :
                                                                 Program->subcmd->data;
        const size_t len = cmds && cmds->subcmd != NULL ? cmds->subcmd->len : Program->subcmd->len;

        if (opt == NULL) return ArgNotDefined;

        for (size_t i = 0; i < len; i++) {
                const char *cmd = opt[i].name;
                const size_t cmd_arg_idx = opt[i].args;
                const char *cmd_arg = Program->args->data[i].name;
                const CrownCommand *cmd_subcmd =
                        opt[i].subcmd && opt[i].subcmd->data != NULL ? opt[i].subcmd->data : NULL;

                if (cmd != NULL && aoc_cstr_eq(optcur, cmd)) {
                        if (cmd_arg != NULL) {
                                optarg = crown_getarg(argv, argc);
                                if (cmd_subcmd != NULL) return i;
                                if (optarg == NULL || optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }
        }
        return ArgNotDefined;
}

AOCLIBS_PREFIX void crown_iprint(const char *msg, int indent) {
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
                w.ws_col = 80;
        }
        const int WIDTH = w.ws_col;

        CROWN_PRINTF("%-*s", indent, "");
        int line_pos = indent;

        const char *START = msg;
        const char *END = msg;

        while (*END) {
                if (*END == '\n') {
                        fputc('\n', CROWN_OUTPUT);
                        CROWN_PRINTF("%*s", indent, "");
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
                        CROWN_PRINTF("\n%*s", indent, "");
                        line_pos = indent;
                }

                CROWN_PRINTF("%.*s", word_len, START);
                line_pos += word_len;

                if (*END && line_pos < WIDTH) {
                        fputc(' ', CROWN_OUTPUT);
                        line_pos++;
                }
        }
        fputc(' ', CROWN_OUTPUT);
}
#endif // AOCLIBS_CROWN

#endif // AOCLIBS_CROWN_H_
