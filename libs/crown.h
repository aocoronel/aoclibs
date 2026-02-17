#ifndef AOCLIBS_CROWN_H_
#define AOCLIBS_CROWN_H_

#include "base.h"
#include "arena.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CROWN_BUFFER 256 // used by crown_normalize_name
#define CROWN_INDENTATION 10 // Indentation by spaces

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
        const CrownArgument *args;
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
        const CrownArgument *args;
        const char *desc;
        CrownCmds *subcmd;
        CrownOpts *flags;
};

typedef struct {
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

CrownCmds ProgramCommand = { 0 };
CrownOpts ProgramOption = { 0 };
CrownArgs ProgramArgument = { 0 };

CrownProgram *Program;
Arena Program_Arena = { 0 };

static CrownCommand *last_cmd = NULL;

// Important to initialize, before anything at the beginning of the main()
// You may set .name, .usage and .desc here.
#define aoc_crown_init(...)                                            \
        do {                                                           \
                CrownProgram *_Program = alloca(sizeof(CrownProgram)); \
                *_Program = (CrownProgram){ .subcmd = &ProgramCommand, \
                                            .flags = &ProgramOption,   \
                                            .args = &ProgramArgument,  \
                                            __VA_ARGS__ };             \
                Program = _Program;                                    \
        } while (0)

// You can either use as global or local variable
// It's interesting to use this to "register" a new argument, so you can simply
// provide a pointer to CrownOption or CrownCommand, potentially reusing the same
// argument for different options and commands
#define aoc_crown_new_arg(key, ...) CrownArgument key = { __VA_ARGS__ }

#define aoc_crown_new_opt(key, opt, ...)      \
        size_t key##_opt = (opt)->flags->len; \
        aoc_crown_append(CrownOption, (opt)->flags, (CrownOption){ __VA_ARGS__ })

// aoc_crown_new_cmd(open, Program, ...)
#define aoc_crown_new_cmd(key, opt, ...)                                                      \
        size_t key##_cmd = (opt)->subcmd->len;                                                \
        do {                                                                                  \
                aoc_crown_append(CrownCommand, (opt)->subcmd, (CrownCommand){ __VA_ARGS__ }); \
                last_cmd = &(opt)->subcmd->data[(opt)->subcmd->len - 1];                      \
                last_cmd->subcmd = aoc_arena_calloc(&Program_Arena, sizeof(CrownCmds));       \
                last_cmd->flags = aoc_arena_calloc(&Program_Arena, sizeof(CrownOpts));        \
        } while (0);                                                                          \
        CrownCommand *key = &(opt)->subcmd->data[key##_cmd];

// Syntactic sugar for aoc_crown_new_opt(last_cmd->flags, ...)
#define aoc_crown_new_subopt(key, ...)                                                        \
        size_t key##_opt = last_cmd->flags->len;                                              \
        do {                                                                                  \
                assert(last_cmd != NULL);                                                     \
                aoc_crown_append(CrownOption, last_cmd->flags, (CrownOption){ __VA_ARGS__ }); \
        } while (0)

#define aoc_crown_new_subcmd(key, ...)                                                           \
        size_t key##_cmd = last_cmd->subcmd->len;                                                \
        do {                                                                                     \
                assert(last_cmd != NULL);                                                        \
                aoc_crown_append(CrownCommand, last_cmd->subcmd, (CrownCommand){ __VA_ARGS__ }); \
                last_cmd->subcmd->data[key##_cmd].subcmd =                                       \
                        aoc_arena_calloc(&Program_Arena, sizeof(CrownCmds));                     \
                last_cmd->subcmd->data[key##_cmd].flags =                                        \
                        aoc_arena_calloc(&Program_Arena, sizeof(CrownOpts));                     \
        } while (0);                                                                             \
        CrownCommand *key = &last_cmd->subcmd->data[key##_cmd];

// Internal macro
#define aoc_crown_append(T, opt, ...)                                      \
        do {                                                               \
                T _tmp = __VA_ARGS__;                                      \
                void *ptr = &_tmp;                                         \
                aoc_arena_da_reserve(&Program_Arena, opt, (opt)->len + 1); \
                memcpy(&(opt)->data[(opt)->len], ptr, sizeof(T));          \
                (opt)->len += 1;                                           \
        } while (0)

/*
 * Prints indented message
 *
 * If the size of the terminal cannot be obtained, fallback to 80 columns
*/
AOCLIBS_PREFIX void aoc_crown_iprint(const char *msg, int indent);

/*
 * Generates bash completions for commands, flags and arguments
*/
AOCLIBS_PREFIX void aoc_crown_bashgen(const CrownEnv *env, size_t envc);

/*
 * Normalizes a strstrg strto a valid shell function name, by replacstrg
 * non-alphanumeric characters with underscores.
 *
 * May truncate to buff_size
*/
AOCLIBS_PREFIX void aoc_crown_normalize_name(char *buff, const char *str, size_t buff_size);

/*
 * Prints help message
*/
AOCLIBS_PREFIX void aoc_crown_help(CrownCommand *null cmd);

/*
 * Generates bash completions for commands, flags and arguments
*/
AOCLIBS_PREFIX void aoc_crown_zshgen(const CrownEnv *env, size_t envc);

/*
 * Get next argument from argv
*/
AOCLIBS_PREFIX char *aoc_crown_getarg(char *argv[], int argc);

// Important errors when using getopt:

enum {
        // Returned when CrownOption.args is not NULL, and no argument has been found.
        ArgMissingOptarg = -1,
        // Returned when input is not recognized to be a flag.
        // This can be used to parse commands and positional arguments.
        ArgNotOpt,
        // This is relevant only for developers, and notices when there was an error
        // on defining and option.
        ArgNotDefined,
        // This should NEVER return. It only returns if optind is higher than argc.
        EndOfArgs,
        // User attempted to enter an option that doesn't exist.
        ArgNotFound,
};

/*
 * Flag parser. If "cmds" is NULL, fallsback to flags defined in Programs.
 * By providing a specific command, getopt will return the index, related to
 * the options defined in the provided command.
*/
AOCLIBS_PREFIX int aoc_crown_getopt(CrownCommand *null cmds, char *argv[], int argc);

/*
 * Command parser. If "cmds" is NULL, fallsback to commands defined in Programs.
 * By providing a specific command, getcmd will return the index, related to
 * the subcommand defined in the provided command.
*/
AOCLIBS_PREFIX int aoc_crown_getcmd(CrownCommand *null cmds, char *argv[], int argc);

#ifdef CROWN_IMPLEMENTATION

        #include <ctype.h>
        #include "rc.h"
        #include "cstr.h"
        #include "colors.h"

// TODO: Subcommands and subcommand options
AOCLIBS_PREFIX void aoc_crown_bashgen(const CrownEnv *env, size_t envc) {
        puts("#!/usr/bin/env bash");

        // Sets all environment variables to the top
        for (size_t i = 0; i < envc; i++)
                fprintf(CROWN_OUTPUT, "%s=%s\n", env[i].name, env[i].value);

        // Generates all the functions responsible for the completions
        //
        // Completes using ls:
        // args.completion = "ls"
        for (size_t i = 0; i < Program->args->len; i++) {
                CrownArgument args = Program->args->data[i];
                char ARG[CROWN_BUFFER];
                if (args.completion)
                        aoc_crown_normalize_name(ARG, args.name, CROWN_BUFFER);
                else
                        return;
                fprintf(CROWN_OUTPUT, "_%s() {\n  %s\n}\n", ARG, args.completion);
        }

        // Main function
        fprintf(CROWN_OUTPUT, "_%s() {\n", Program->name);
        fprintf(CROWN_OUTPUT, "  local cur prev\n");
        fprintf(CROWN_OUTPUT, "  cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");
        fprintf(CROWN_OUTPUT, "  prev=\"${COMP_WORDS[COMP_CWORD-1]}\"\n");
        fprintf(CROWN_OUTPUT, "  COMPREPLY=()\n");

        // Flag completion
        fprintf(CROWN_OUTPUT, "  if [[ \"${cur}\" == -* ]]; then\n");
        fprintf(CROWN_OUTPUT, "    mapfile -t COMPREPLY < <(compgen -W \"");

        putchar(' ');
        for (size_t i = 0; i < Program->flags->len; i++) {
                CrownOption flags = Program->flags->data[i];
                if (flags.long_opt != NULL) fprintf(CROWN_OUTPUT, " %s", flags.long_opt);
                if (flags.short_opt != NULL) fprintf(CROWN_OUTPUT, " %s", flags.short_opt);
        }

        fprintf(CROWN_OUTPUT, "\" -- \"${cur}\")\n");
        fprintf(CROWN_OUTPUT, "    return 0\n");
        fprintf(CROWN_OUTPUT, "  fi\n");

        // Argument completion
        fprintf(CROWN_OUTPUT, "  case \"${prev}\" in\n");
        // Commands
        for (size_t i = 0; i < Program->subcmd->len; i++) {
                char ARG[CROWN_BUFFER];
                CrownCommand cmds = Program->subcmd->data[i];
                if (cmds.args && cmds.args->completion)
                        aoc_crown_normalize_name(ARG, cmds.args->name, CROWN_BUFFER);
                else
                        return;
                fprintf(CROWN_OUTPUT, "  %s)\n", cmds.name);
                fprintf(CROWN_OUTPUT,
                        "    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n",
                        ARG);
                fprintf(CROWN_OUTPUT, "    return 0\n    ;;\n");
        }
        // Flags
        for (size_t i = 0; i < Program->flags->len; i++) {
                CrownOption flags = Program->flags->data[i];
                char ARG[CROWN_BUFFER];
                if (flags.args && flags.args->completion)
                        aoc_crown_normalize_name(ARG, flags.args->name, CROWN_BUFFER);
                else
                        return;
                const char *SHORT_FLAG = flags.short_opt;
                const char *LONG_FLAG = flags.long_opt;
                if (LONG_FLAG && SHORT_FLAG)
                        fprintf(CROWN_OUTPUT, "  %s|%s)\n", SHORT_FLAG, LONG_FLAG);
                else if (LONG_FLAG)
                        fprintf(CROWN_OUTPUT, "  %s)\n", LONG_FLAG);
                else if (SHORT_FLAG)
                        fprintf(CROWN_OUTPUT, "  %s)\n", SHORT_FLAG);
                else
                        return;
                fprintf(CROWN_OUTPUT,
                        "    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n",
                        ARG);
                fprintf(CROWN_OUTPUT, "    return 0\n    ;;\n");
        }
        fprintf(CROWN_OUTPUT, "  esac\n");

        // Command completion
        fprintf(CROWN_OUTPUT, "  mapfile -t COMPREPLY < <(compgen -W \"");

        for (size_t i = 0; i < Program->subcmd->len; i++) {
                CrownCommand cmds = Program->subcmd->data[i];
                fprintf(CROWN_OUTPUT, " %s", cmds.name);
        }

        fprintf(CROWN_OUTPUT, "\" -- \"${cur}\")\n");
        fprintf(CROWN_OUTPUT, "  return 0\n");
        fprintf(CROWN_OUTPUT, "}\n");

        // Assign function to program
        fprintf(CROWN_OUTPUT, "complete -F _%s %s\n", Program->name, Program->name);
}

AOCLIBS_PREFIX void aoc_crown_normalize_name(char *buff, const char *str, size_t buff_size) {
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

/*
 * Helper to print headings
*/
internal inline void aoc_print_header(const char *msg, const char *style) {
        fprintf(CROWN_OUTPUT, "%s%s%s", style, msg, COLOR_RESET);
}

/*
 * Helper qsort to sort commands
*/
internal inline int aoc_crown_help_qsort_cmd(const void *a, const void *b) {
        const CrownCommand *CMD_A = (const CrownCommand *)a;
        const CrownCommand *CMD_B = (const CrownCommand *)b;

        return strcmp(CMD_A->name, CMD_B->name);
}

internal inline const char *qsort_get_opt(const CrownOption *flag) {
        if (flag->short_opt) return flag->short_opt;
        if (flag->long_opt) return flag->long_opt;
        return "";
}

/*
 * Helper qsort to sort options
*/
internal inline int aoc_crown_help_qsort_opt(const void *a, const void *b) {
        const CrownOption *FLAG_A = (const CrownOption *)a;
        const CrownOption *FLAG_B = (const CrownOption *)b;

        return strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

internal inline bool aoc_has_commands(CrownCommand *cmds) {
        CrownCmds *cmd = cmds && cmds->subcmd != NULL ? cmds->subcmd : Program->subcmd;
        if (cmd == NULL) return false;
        if (cmd->len == 0) return false;
        #ifndef NDEBUG
        ASSERT(cmd->data != NULL, "No command is defined, but length is %d\n", cmd->len);
        for (size_t i = 0; i < cmd->len; i++)
                ASSERT(cmd->data[i].name != NULL,
                       "Found a subcommand without name in %s",
                       cmds->name);
        #endif
        return true;
}

internal inline bool aoc_has_options(CrownCommand *cmds) {
        CrownOpts *opt = cmds && cmds->flags != NULL ? cmds->flags : Program->flags;
        if (opt == NULL) return false;
        if (opt->len == 0) return false;
        #ifndef NDEBUG
        ASSERT(opt->data != NULL, "No option is defined, but length is %d\n", opt->len);
        for (size_t i = 0; i < opt->len; i++)
                ASSERT(opt->data[i].short_opt != NULL || opt->data[i].long_opt != NULL,
                       "Found a option without a least one short/long flag in %s",
                       cmds->name);
        #endif
        return true;
}

        // Used in crown_help_commands and crown_help
        #define aoc_crown_help_command(command)                                            \
                do {                                                                       \
                        if ((command)->args) {                                             \
                                fprintf(CROWN_OUTPUT,                                      \
                                        "  %s%s%s [%s]\n",                                 \
                                        COLOR_BOLD,                                        \
                                        (command)->name,                                   \
                                        COLOR_RESET,                                       \
                                        (command)->args && (command)->args->name != NULL ? \
                                                (command)->args->name :                    \
                                                "");                                       \
                        } else {                                                           \
                                fprintf(CROWN_OUTPUT,                                      \
                                        "  %s%s%s\n",                                      \
                                        COLOR_BOLD,                                        \
                                        (command)->name,                                   \
                                        COLOR_RESET);                                      \
                        }                                                                  \
                } while (0)

// Commands:
//   cmd1 <ARG>
//       Description
//   cmd2 [ARG]
//       Description
internal inline void aoc_crown_help_commands(CrownCommand *cmds) {
        aoc_print_header("Commands:\n", COLOR_BOLD_UNDERLINE);

        const CrownCmds *COMMAND = cmds && cmds->subcmd != NULL ? cmds->subcmd : Program->subcmd;

        for (size_t i = 0; i < COMMAND->len; i++) {
                if (COMMAND->data == NULL) continue;
                CrownCommand cmd = COMMAND->data[i];

                aoc_crown_help_command(&cmd);

                if (cmd.desc && cmd.desc[0] != '\0') {
                        aoc_crown_iprint(cmd.desc, CROWN_INDENTATION);
                        fputc('\n', CROWN_OUTPUT);
                }
        }
        fputc('\n', CROWN_OUTPUT);
}

        #define aoc_crown_help_option(opt)                                                  \
                do {                                                                        \
                        const char *SHORT_OPT = (opt)->short_opt;                           \
                        const char *LONG_OPT = (opt)->long_opt;                             \
                        if (SHORT_OPT && LONG_OPT) {                                        \
                                fprintf(CROWN_OUTPUT,                                       \
                                        "  %s%s%s, %s%s%s",                                 \
                                        COLOR_BOLD,                                         \
                                        SHORT_OPT,                                          \
                                        COLOR_RESET,                                        \
                                        COLOR_BOLD,                                         \
                                        LONG_OPT,                                           \
                                        COLOR_RESET);                                       \
                        } else if (LONG_OPT) {                                              \
                                fprintf(CROWN_OUTPUT,                                       \
                                        "  %s%s%s",                                         \
                                        COLOR_BOLD,                                         \
                                        LONG_OPT,                                           \
                                        COLOR_RESET);                                       \
                        } else if (SHORT_OPT) {                                             \
                                fprintf(CROWN_OUTPUT,                                       \
                                        "  %s%s%s",                                         \
                                        COLOR_BOLD,                                         \
                                        SHORT_OPT,                                          \
                                        COLOR_RESET);                                       \
                        } else {                                                            \
                                continue;                                                   \
                        }                                                                   \
                        if ((opt)->args) fprintf(CROWN_OUTPUT, " [%s]", (opt)->args->name); \
                } while (0)

// Options:
//   -s, --short [ARG]
//   -l, --long [ARG]
//   -h, --help
//       Description
internal inline void aoc_crown_help_options(CrownCommand *cmds) {
        aoc_print_header("Options:\n", COLOR_BOLD_UNDERLINE);

        const CrownOpts *FLAG = cmds && cmds->flags != NULL ? cmds->flags : Program->flags;

        for (size_t i = 0; i < FLAG->len; i++) {
                if (FLAG->data == NULL) continue;
                CrownOption flags = FLAG->data[i];

                aoc_crown_help_option(&flags);

                fputc('\n', CROWN_OUTPUT);
                if (flags.desc && flags.desc[0] != '\0') {
                        aoc_crown_iprint(flags.desc, CROWN_INDENTATION);
                        fputc('\n', CROWN_OUTPUT);
                }
        }
        fputc('\n', CROWN_OUTPUT);
}

AOCLIBS_PREFIX void aoc_crown_help(CrownCommand *null cmd) {
        // NULL in case you want to print the general flags and commands

        CrownCommand *print = cmd == NULL ? NULL : cmd;
        if (cmd == NULL) {
                qsort(Program->subcmd->data,
                      Program->subcmd->len,
                      sizeof(CrownCommand),
                      aoc_crown_help_qsort_cmd);
                qsort(Program->flags->data,
                      Program->flags->len,
                      sizeof(CrownOption),
                      aoc_crown_help_qsort_opt);

                // program name | program description
                fprintf(CROWN_OUTPUT, "%s | %s\n\n", Program->name, Program->desc);

                // Usage: program usage
                printf("  %s%s%s %s\n\n", COLOR_BOLD, Program->name, COLOR_RESET, Program->usage);
        } else {
                qsort(cmd->subcmd->data,
                      cmd->subcmd->len,
                      sizeof(CrownCommand),
                      aoc_crown_help_qsort_cmd);
                qsort(cmd->flags->data,
                      cmd->flags->len,
                      sizeof(CrownOption),
                      aoc_crown_help_qsort_opt);

                // command name | command description
                fprintf(CROWN_OUTPUT, "%s\n\n", cmd->desc);

                aoc_print_header("Usage:", COLOR_BOLD_UNDERLINE);
                aoc_crown_help_command(cmd);
                fputc('\n', CROWN_OUTPUT);
        }
        if (aoc_has_commands(print)) aoc_crown_help_commands(print);
        if (aoc_has_options(print)) aoc_crown_help_options(print);
}

// crown_zshgen

internal inline void aoc_crown_zshgen_print_arg_autocomplete(const CrownArgument *args) {
        char ARG[CROWN_BUFFER] = { 0 };
        if (args)
                aoc_crown_normalize_name(ARG, args->name, CROWN_BUFFER);
        else
                return;
        const char *COMPLETIONS = args->completion;
        if (COMPLETIONS != NULL) {
                fprintf(CROWN_OUTPUT, "_%s_get_%s() {\n", Program->name, ARG);
                fprintf(CROWN_OUTPUT, "  local results\n");
                fprintf(CROWN_OUTPUT, "  results=(${(f)\"$(%s 2>/dev/null)\"})\n", COMPLETIONS);
                fprintf(CROWN_OUTPUT, "  compadd -Q -a results\n");
                fprintf(CROWN_OUTPUT, "}\n\n");
        }
}

internal void aoc_crown_zshgen_print_flag_arg(const CrownOption *flag) {
        char ARG[CROWN_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                aoc_crown_normalize_name(ARG, flag->args->name, CROWN_BUFFER);
        else
                return;
        const char *DESC = flag->desc;
        const char *COMP = (flag->args) ? flag->args->completion : NULL;

        if (!COMP) return;

        if (LONG_FLAG) {
                fprintf(CROWN_OUTPUT, "    '%s", LONG_FLAG);
                if (DESC) fprintf(CROWN_OUTPUT, "=[%s]", DESC);
                if (COMP) {
                        fprintf(CROWN_OUTPUT, ":%s:_%s_get_%s", ARG, Program->name, ARG);
                } else {
                        fprintf(CROWN_OUTPUT, ":%s", ARG);
                }
                fprintf(CROWN_OUTPUT, "' \\\n");
        }
        if (SHORT_FLAG) {
                fprintf(CROWN_OUTPUT, "    '%s", SHORT_FLAG);
                if (DESC) fprintf(CROWN_OUTPUT, "[%s]", DESC);
                if (COMP) {
                        fprintf(CROWN_OUTPUT, ":%s:_%s_get_%s", ARG, Program->name, ARG);
                } else {
                        fprintf(CROWN_OUTPUT, ":%s", ARG);
                }
                fprintf(CROWN_OUTPUT, "' \\\n");
        }
}

internal inline void aoc_crown_zshgen_print_command_case(const CrownCommand *cmd) {
        char ARG[CROWN_BUFFER];
        if (cmd->args)
                aoc_crown_normalize_name(ARG, cmd->args->name, CROWN_BUFFER);
        else
                return;
        const char *COMP = (cmd->args) ? cmd->args->completion : NULL;
        if (!COMP) return;

        fprintf(CROWN_OUTPUT, "        %s)\n", cmd->name);
        fprintf(CROWN_OUTPUT, "          _arguments \\\n");
        fprintf(CROWN_OUTPUT, "            '*:%s:_%s_get_%s' \\\n", ARG, Program->name, ARG);
        fprintf(CROWN_OUTPUT, "          ;;\n");
}

internal inline void aoc_crown_zshgen_print_flag_case(const CrownOption *flag) {
        char ARG[CROWN_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                aoc_crown_normalize_name(ARG, flag->args->name, CROWN_BUFFER);
        else
                return;
        const char *COMPLETIONS = (flag->args) ? flag->args->completion : NULL;
        if (!COMPLETIONS) return;

        if (SHORT_FLAG != NULL && LONG_FLAG != NULL) {
                fprintf(CROWN_OUTPUT, "        %s | %s)\n", SHORT_FLAG, LONG_FLAG);
        } else if (SHORT_FLAG != NULL) {
                fprintf(CROWN_OUTPUT, "        %s)\n", SHORT_FLAG);
        } else if (LONG_FLAG != NULL) {
                fprintf(CROWN_OUTPUT, "        %s)\n", LONG_FLAG);
        }
        fprintf(CROWN_OUTPUT, "          _arguments \\\n");
        fprintf(CROWN_OUTPUT, "            '*:%s:_%s_get_%s' \\\n", ARG, Program->name, ARG);
        fprintf(CROWN_OUTPUT, "          return\n");
        fprintf(CROWN_OUTPUT, "          ;;\n");
}

// TODO: Subcommands and subcommand options
AOCLIBS_PREFIX void aoc_crown_zshgen(const CrownEnv *env, size_t envc) {
        // Header
        fprintf(CROWN_OUTPUT, "#compdef %s\n\n", Program->name);

        // Environment defaults
        for (size_t i = 0; i < envc; i++) {
                fprintf(CROWN_OUTPUT, "%s=%s\n", env[i].name, env[i].value);
        }

        // Main function
        fprintf(CROWN_OUTPUT, "_%s() {\n", Program->name);
        fprintf(CROWN_OUTPUT, "  local -a subcommands\n\n");

        // Define Subcommands
        fprintf(CROWN_OUTPUT, "  subcommands=(\n");
        for (size_t i = 0; i < Program->subcmd->len; i++) {
                CrownCommand cmds = Program->subcmd->data[i];
                fprintf(CROWN_OUTPUT, "    \"%s:%s\"\n", cmds.name, cmds.desc);
        }
        fprintf(CROWN_OUTPUT, "  )\n\n");

        // Define arguments
        fprintf(CROWN_OUTPUT, "  _arguments -C \\\n");
        fprintf(CROWN_OUTPUT, "    '1:command:->subcmds' \\\n");
        for (size_t i = 0; i < Program->flags->len; i++) {
                CrownOption flags = Program->flags->data[i];
                aoc_crown_zshgen_print_flag_arg(&flags);
        }
        fprintf(CROWN_OUTPUT, "    '*::args:->command_args'\n\n");

        // Autocompletion
        fprintf(CROWN_OUTPUT, "  case $state in\n");
        fprintf(CROWN_OUTPUT, "    subcmds)\n");
        fprintf(CROWN_OUTPUT, "      _describe 'command' subcommands\n");
        fprintf(CROWN_OUTPUT, "      return\n");
        fprintf(CROWN_OUTPUT, "      ;;\n");
        fprintf(CROWN_OUTPUT, "    command_args)\n");
        fprintf(CROWN_OUTPUT, "      case $words[1] in\n");

        // Autocomplete arguments from commands
        for (size_t i = 0; i < Program->subcmd->len; i++) {
                CrownCommand cmds = Program->subcmd->data[i];
                aoc_crown_zshgen_print_command_case(&cmds);
        }
        // Autocomplete arguments from flags
        for (size_t i = 0; i < Program->flags->len; i++) {
                CrownOption flags = Program->flags->data[i];
                aoc_crown_zshgen_print_flag_case(&flags);
        }
        fprintf(CROWN_OUTPUT, "      esac\n");
        fprintf(CROWN_OUTPUT, "      ;;\n");
        fprintf(CROWN_OUTPUT, "  esac\n");
        fprintf(CROWN_OUTPUT, "}\n\n");

        // Define helper functions to autocomplete arguments
        for (size_t i = 0; i < Program->args->len; i++) {
                CrownArgument args = Program->args->data[i];
                aoc_crown_zshgen_print_arg_autocomplete(&args);
        }

        // Assign function to program
        fprintf(CROWN_OUTPUT, "compdef _%s %s\n", Program->name, Program->name);
}

// CLI Argument Parser

AOCLIBS_PREFIX char *aoc_crown_getarg(char *argv[], int argc) {
        if (optind >= argc) return NULL;
        return argv[optind++];
}

        #define aoc_crown_parseopt(opt, argv, argc) aoc_crown_getopt((opt), (argv), (argc))

AOCLIBS_PREFIX int aoc_crown_getopt(CrownCommand *null cmds, char *argv[], int argc) {
        const char *arg = aoc_crown_getarg(argv, argc);
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
                const CrownArgument *flag_arg = opt[i].args;

                if (long_opt != NULL && aoc_cstr_eq(arg, long_opt)) {
                        if (flag_arg != NULL) {
                                optarg = aoc_crown_getarg(argv, argc);
                                if (optarg == NULL || optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }

                if (short_opt != NULL && aoc_cstr_eq(arg, short_opt)) {
                        if (flag_arg != NULL) {
                                optarg = aoc_crown_getarg(argv, argc);
                                if (optarg == NULL || optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }
        }
        return ArgNotFound;
}

        #define aoc_subcmd(opt, idx) (opt)->subcmd->data[(idx)]

        #define aoc_crown_parsecmd(opt, argv, argc) aoc_crown_getcmd((opt), (argv), (argc))

AOCLIBS_PREFIX int aoc_crown_getcmd(CrownCommand *null cmds, char *argv[], int argc) {
        const CrownCommand *opt = cmds && cmds->subcmd != NULL ? cmds->subcmd->data :
                                                                 Program->subcmd->data;
        const size_t len = cmds && cmds->subcmd != NULL ? cmds->subcmd->len : Program->subcmd->len;

        if (opt == NULL) return ArgNotDefined;

        for (size_t i = 0; i < len; i++) {
                const char *cmd = opt[i].name;
                const CrownArgument *cmd_arg = opt[i].args;
                const CrownCommand *cmd_subcmd =
                        opt[i].subcmd && opt[i].subcmd->data != NULL ? opt[i].subcmd->data : NULL;

                if (cmd != NULL && aoc_cstr_eq(optcur, cmd)) {
                        if (cmd_arg != NULL) {
                                optarg = aoc_crown_getarg(argv, argc);
                                if (cmd_subcmd != NULL) return i;
                                if (optarg == NULL || optarg[0] == '-') return ArgMissingOptarg;
                        }
                        return i; // Success
                }
        }
        return ArgNotDefined;
}

AOCLIBS_PREFIX void aoc_crown_deinit(void) {
        aoc_arena_destroy(&Program_Arena);
}

AOCLIBS_PREFIX void aoc_crown_iprint(const char *msg, int indent) {
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
                w.ws_col = 80;
        }
        const int WIDTH = w.ws_col;

        fprintf(CROWN_OUTPUT, "%-*s", indent, "");
        int line_pos = indent;

        const char *START = msg;
        const char *END = msg;

        while (*END) {
                if (*END == '\n') {
                        fputc('\n', CROWN_OUTPUT);
                        fprintf(CROWN_OUTPUT, "%*s", indent, "");
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
                        fprintf(CROWN_OUTPUT, "\n%*s", indent, "");
                        line_pos = indent;
                }

                fprintf(CROWN_OUTPUT, "%.*s", word_len, START);
                line_pos += word_len;

                if (*END && line_pos < WIDTH) {
                        fputc(' ', CROWN_OUTPUT);
                        line_pos++;
                }
        }
        fputc(' ', CROWN_OUTPUT);
}
#endif // CROWN_IMPLEMENTATION

#ifdef AOCLIBS_STRIP_PREFIX
        #define crown_iprint aoc_crown_iprint
        #define crown_bashgen aoc_crown_bashgen
        #define crown_normalize_name aoc_crown_normalize_name
        #define crown_help aoc_crown_help
        #define crown_zshgen aoc_crown_zshgen
        #define crown_getarg aoc_crown_getarg

        #define crown_parseopt aoc_crown_parseopt
        #define crown_parsecmd aoc_crown_parsecmd

        #define crown_init aoc_crown_init
        #define crown_deinit aoc_crown_deinit

        #define crown_new_arg aoc_crown_new_arg
        #define crown_new_opt aoc_crown_new_opt
        #define crown_new_cmd aoc_crown_new_cmd

        #define crown_new_subopt aoc_crown_new_subopt
        #define crown_new_subcmd aoc_crown_new_subcmd
#endif

#endif // AOCLIBS_CROWN_H_
