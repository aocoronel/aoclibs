#ifndef AOCLIBS_CROWN_H_
#define AOCLIBS_CROWN_H_

#include "arena.h"
#include "base.h"
#include "cstr.c"
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
    const int args;
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
    const int args;
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
} CrownProgram;

CrownCmds Crown_Command = { 0 };
CrownOpts Crown_Option = { 0 };
CrownArgs Crown_Argument = { 0 };

CrownProgram *Program;
Arena Program_Arena = { 0 };

static CrownCommand *last_cmd = NULL;

#define CROWN_PRINTF(...) fprintf(CROWN_OUTPUT, __VA_ARGS__)
#define CROWN_PUTS(string) fputs(string, CROWN_OUTPUT)
#define CROWN_PUTC(c) fputc(c, CROWN_OUTPUT)

// Important to initialize, before anything at the beginning of the main()
// You may set .name, .usage and .desc here.
#define crown_init(...)                                                                            \
    do {                                                                                           \
        CrownProgram *_Program = alloca(sizeof(CrownProgram));                                     \
        *_Program = (CrownProgram){                                                                \
            .subcmd = &Crown_Command, .flags = &Crown_Option, .args = &Crown_Argument, __VA_ARGS__ \
        };                                                                                         \
        Program = _Program;                                                                        \
        Program->args->len = 1;                                                                    \
    } while (0)

// Once you no longer need to use Crown, you can deinitialize all memory it used.
AOCDEF void crown_deinit(void);

// Add new argument
// It has a key which is used to be referenced by flags and commands
#define crown_new_arg(key, ...)      \
    size_t key = Program->args->len; \
    crown_append(CrownArgument, Program->args, (CrownArgument){ __VA_ARGS__ })

#define crown_new_opt(key, opt, ...)                                           \
    size_t key##_id = (opt)->flags->len;                                       \
    do {                                                                       \
        crown_append(CrownOption, (opt)->flags, (CrownOption){ __VA_ARGS__ }); \
    } while (0);                                                               \
    CrownOption *key = &(opt)->flags->data[key##_id];

// crown_new_cmd(open, Program, ...)
#define crown_new_cmd(key, opt, ...)                                              \
    size_t key##_id = (opt)->subcmd->len;                                         \
    do {                                                                          \
        crown_append(CrownCommand, (opt)->subcmd, (CrownCommand){ __VA_ARGS__ }); \
        last_cmd = &(opt)->subcmd->data[(opt)->subcmd->len - 1];                  \
        last_cmd->subcmd = arena_calloc(&Program_Arena, sizeof(CrownCmds));       \
        last_cmd->flags = arena_calloc(&Program_Arena, sizeof(CrownOpts));        \
    } while (0);                                                                  \
    CrownCommand *key = &(opt)->subcmd->data[key##_id];

#define crown_help_flag(key)               \
    crown_new_subopt(key,                  \
                     .short_opt = "-h",    \
                     .long_opt = "--help", \
                     .args = CrownNoArg,   \
                     .desc = "Display this message and exits");

// Syntactic sugar for crown_new_opt(last_cmd->flags, ...)
#define crown_new_subopt(key, ...)                                                \
    size_t key##_id = last_cmd->flags->len;                                       \
    do {                                                                          \
        ASSERT(last_cmd != NULL, "No command defined");                           \
        crown_append(CrownOption, last_cmd->flags, (CrownOption){ __VA_ARGS__ }); \
    } while (0);                                                                  \
    CrownOption *key = &last_cmd->flags->data[key##_id];

#define crown_new_subcmd(key, ...)                                                                 \
    size_t key##_id = last_cmd->subcmd->len;                                                       \
    do {                                                                                           \
        ASSERT(last_cmd != NULL, "No command defined");                                            \
        crown_append(CrownCommand, last_cmd->subcmd, (CrownCommand){ __VA_ARGS__ });               \
        last_cmd->subcmd->data[key##_id].subcmd = arena_calloc(&Program_Arena, sizeof(CrownCmds)); \
        last_cmd->subcmd->data[key##_id].flags = arena_calloc(&Program_Arena, sizeof(CrownOpts));  \
    } while (0);                                                                                   \
    CrownCommand *key = &last_cmd->subcmd->data[key##_id];

// Internal macro
#define crown_append(T, opt, ...)                         \
    do {                                                  \
        T _tmp = __VA_ARGS__;                             \
        void *ptr = &_tmp;                                \
        dar_reserve(&Program_Arena, opt, (opt)->len + 1); \
        memcpy(&(opt)->data[(opt)->len], ptr, sizeof(T)); \
        (opt)->len += 1;                                  \
    } while (0)

// Prints indented message of given "indent".
//
// If the size of the terminal cannot be obtained, fallback to 80 columns.
AOCDEF void crown_iprint(const char *msg, int indent);

// Generates bash completions for commands, flags and arguments.
//
// If user provides environment variables, they will be set as a global completion variable.
// This way the user can write argument completion that uses those environment variables.
AOCDEF void crown_bashgen(const CrownEnv *null env, size_t envc);

// Normalizes a given "str" to a valid shell function name, by replacing it to non-alphanumeric
// characters with underscores.
//
// May truncate to "buff_size".
AOCDEF void crown_normalize_name(char *buff, const char *str, size_t buff_size);

// Prints help message.
//
// If "cmd" is NULL, prints the main help message.
AOCDEF void crown_help(CrownCommand *null cmd);

// Get next argument from argv.
AOCDEF char *crown_getarg(char *argv[], int argc);

// Important errors when using getopt:

enum {
    // Command or flag doesn't have an assigned argument
    CrownNoArg = 0,
    // Returned when CrownOption.args is not NULL, and no argument has been found.
    CrownMissingOptarg = -1,
    // Returned when input is not recognized to be a flag.
    // This can be used to parse commands and positional arguments.
    CrownNotOpt = -2,
    // This is relevant only for developers, and notices when there was an error
    // on defining and option or command.
    CrownNotDefined = -3,
    // This should NEVER return. It only returns if optind is higher than argc.
    CrownEndOfArgs = -4,
    // User attempted to enter an option that doesn't exist.
    CrownNotFound = -5,
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
AOCDEF int crown_getopt(CrownCommand *null cmds, char *argv[], int argc);

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
AOCDEF int crown_getcmd(CrownCommand *null cmds, char *argv[], int argc);

// Dump the entire CrownProgram variable in a stack-based format, so the user can initialize it using
// the arena, and at release switch to the stack-based eliminating all allocations made by Crown.
AOCDEF void crown_dump(FILE *fp);

#ifdef AOCLIBS_CROWN

#include "colors.h"
#include "cstr.h"
#include <ctype.h>

AOCDEF void crown_deinit(void) {
    arena_destroy(&Program_Arena);
}

// Helper function
AOCDEF void crown_indent_completion(int indent) {
    for (int i = 0; i < indent; i++) {
        CROWN_PUTC(' ');
    }
}

// Helper function
AOCDEF void crown_bashgen_case_prev_open(int level) {
    CROWN_PRINTF("  case \"${words[%d]}\" in\n", level);
}

// Helper function
AOCDEF void crown_bashgen_case_prev_close(int indent) {
    crown_indent_completion(indent);
    CROWN_PRINTF("  esac\n");
}

// Helper function
AOCDEF void crown_bashgen_options(CrownOpts *cmds, int indent) {
    CrownOpts *curr_cmd = cmds == NULL ? Program->flags : cmds;
    ASSERT(curr_cmd != NULL);

    range(0, curr_cmd->len, i) {
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
            CROWN_PRINTF("    echo \"$(_%s)\"\n", ARG);
        }
        crown_indent_completion(indent);
        CROWN_PRINTF("    return 0\n");
        crown_indent_completion(indent);
        CROWN_PRINTF("    ;;\n");
    }
}

void crown_print_subcmd_completion(CrownCommand cmd, int indent) {
    crown_indent_completion(indent);
    CROWN_PUTS("    comp=(\n");
    range(0, cmd.subcmd->len, j) {
        CrownCommand completion_cmd = cmd.subcmd->data[j];
        bool has_desc = completion_cmd.desc != NULL ? true : false;
        crown_indent_completion(indent);
        if (has_desc)
            CROWN_PRINTF("      \"%s: %s\"\n", completion_cmd.name, completion_cmd.desc);
        else
            CROWN_PRINTF("      \"%s\"\n", completion_cmd.name);
    }
    range(0, cmd.flags->len, j) {
        CrownOption completion_opt = cmd.flags->data[j];
        bool has_desc = completion_opt.desc != NULL ? true : false;
        if (completion_opt.short_opt) {
            crown_indent_completion(indent);
            if (has_desc)
                CROWN_PRINTF("      \"%s: %s\"\n", completion_opt.short_opt, completion_opt.desc);
            else
                CROWN_PRINTF("      \"%s\"\n", completion_opt.short_opt);
        }
        if (completion_opt.long_opt) {
            crown_indent_completion(indent);
            if (has_desc)
                CROWN_PRINTF("      \"%s: %s\"\n", completion_opt.long_opt, completion_opt.desc);
            else
                CROWN_PRINTF("      \"%s\"\n", completion_opt.long_opt);
        }
    }
    crown_indent_completion(indent);
    CROWN_PUTS("    )\n");
    crown_indent_completion(indent);
    CROWN_PUTS("    for elem in \"${comp[@]}\"; do\n");
    crown_indent_completion(indent);
    CROWN_PUTS("      if [[ $elem == \"$current_word\"* ]]; then echo \"$elem\"; fi\n");
    crown_indent_completion(indent);
    CROWN_PUTS("    done\n");
}

// Helper function
AOCDEF void crown_bashgen_subcommand(CrownCmds *cmds, int indent, int level) {
    CrownCmds *curr_cmd = cmds == NULL ? Program->subcmd : cmds;
    ASSERT(curr_cmd != NULL);

    range(0, curr_cmd->len, i) {
        char ARG[CROWN_BUFFER];
        CrownCommand cmd = curr_cmd->data[i];
        CrownArgument arg = Program->args->data[cmd.args];

        bool has_arg = arg.name != NULL && cmd.args != 0 ? true : false;
        bool has_comp = arg.completion != NULL && cmd.args != 0 ? true : false;

        if (arg.name) crown_normalize_name(ARG, arg.name, CROWN_BUFFER);

        crown_indent_completion(indent);
        CROWN_PRINTF("  %s)\n", cmd.name);

        if (cmd.subcmd && cmd.subcmd->data || cmd.flags && cmd.flags->data) {
            crown_indent_completion(indent + 4);
            crown_bashgen_case_prev_open(level + 1);
            range(0, cmd.subcmd->len, j) {
                crown_bashgen_subcommand(cmd.subcmd, indent + 6, level + 1);
            }
            range(0, cmd.flags->len, j) {
                crown_bashgen_options(cmd.flags, indent + 6);
            }
            crown_bashgen_case_prev_close(indent + 4);
        }

        if (has_arg && has_comp) {
            crown_indent_completion(indent);
            CROWN_PRINTF("    echo \"$(_%s)\"\n", ARG);
            if (cmd.subcmd != NULL) {
                eprintf("%s[WARNING]%s The command %s has subcommands and an argument. "
                        "Crown expects to be either one or the other\n",
                        COLOR_YELLOW,
                        COLOR_RESET,
                        cmd.name);
            }
        } else {
            if (cmd.subcmd->len > 0 || cmd.flags->len > 0)
                crown_print_subcmd_completion(cmd, indent);
        }

        if (cmd.subcmd == NULL) indent -= 4;
        crown_indent_completion(indent);
        CROWN_PRINTF("    return 0\n");
        crown_indent_completion(indent);
        CROWN_PRINTF("    ;;\n");
    }
}

AOCDEF void crown_bashgen_env_vars(const CrownEnv *env, size_t envc) {
    if (env == NULL || envc == 0) return;
    ASSERT(envc <= Program->args->len);
    for (size_t j = 0; j < envc; j++) {
        if (env[j].opt == NULL || env[j].opt->args == 0) continue;

        const char *arg_name = Program->args->data[env->opt->args].name;

        ASSERT(arg_name != NULL);
        if (!cstr_eq(arg_name, env->name)) continue;

        CROWN_PUTS("  for ((i = 0; i < ${#words[@]}; i++)); do\n");
        if (env[j].opt->short_opt && env[j].opt->long_opt) {
            CROWN_PRINTF(
                    "    if [[ \"${words[i]}\" == \"%s\" ]] || [[ \"${words[i]}\" == \"%s\" ]] && ((i + 1 < ${#words[@]})); then\n",
                    env[j].opt->short_opt,
                    env[j].opt->long_opt);
        } else if (env[j].opt->short_opt) {
            CROWN_PRINTF(
                    "    if [[ \"${words[i]}\" == \"%s\" ]] && ((i + 1 < ${#words[@]})); then\n",
                    env[j].opt->long_opt);
        } else if (env[j].opt->long_opt) {
            CROWN_PRINTF(
                    "    if [[ \"${words[i]}\" == \"%s\" ]] && ((i + 1 < ${#words[@]})); then\n",
                    env[j].opt->short_opt);
        }
        CROWN_PRINTF("       %s=\"${words[i + 1]}\"\n", env->name);
        CROWN_PUTS("       break\n");
        CROWN_PUTS("    fi\n");
        CROWN_PUTS("  done\n");
        CROWN_PUTC('\n');
    }
}

void crown_generate_completion(const CrownEnv *env, int envc, int default_level) {
    if (default_level == 1)
        CROWN_PUTS("_generate_completions_bash() {\n");
    else if (default_level == 2)
        CROWN_PUTS("_generate_completions_zsh() {\n");

    CROWN_PUTS("  local idx=\"$1\"; shift\n"
               "  local words=( \"$@\" )\n"
               "  local current_word=${words[idx]}\n"
               "  local prev_word=${words[idx - 1]}\n"
               "  local level=${#words[@]}\n"
               "\n");
    CROWN_PUTS("  local global_commands=(\n");
    range(0, Program->subcmd->len, i) {
        CrownCommand completion_cmd = Program->subcmd->data[i];
        const char *completion_desc = completion_cmd.desc;

        bool has_desc = completion_desc != NULL ? true : false;

        if (completion_cmd.name) {
            if (has_desc) {
                CROWN_PRINTF("    \"%s: %s\"\n", completion_cmd.name, completion_desc);
            } else {
                CROWN_PRINTF("    \"%s\"\n", completion_cmd.name);
            }
        }
    }
    CROWN_PUTS("  )\n");
    CROWN_PUTS("  local global_flags=(\n");
    range(0, Program->flags->len, i) {
        CrownOption completion_flag = Program->flags->data[i];
        const char *completion_desc = completion_flag.desc;

        bool has_desc = completion_desc != NULL ? true : false;

        if (completion_flag.long_opt) {
            if (has_desc) {
                CROWN_PRINTF("    \"%s: %s\"\n", completion_flag.long_opt, completion_desc);
            } else {
                CROWN_PRINTF("    \"%s\"\n", completion_flag.long_opt);
            }
        } else if (completion_flag.short_opt) {
            if (has_desc) {
                CROWN_PRINTF("    \"%s: %s\"\n", completion_flag.short_opt, completion_desc);
            } else {
                CROWN_PRINTF("   \"%s\"\n", completion_flag.short_opt);
            }
        }
    }
    CROWN_PUTS("  )\n");
    CROWN_PUTS("\n");

    crown_bashgen_env_vars(env, envc);

    // Argument completion
    CROWN_PRINTF("  case \"${words[%d]}\" in\n", default_level);
    // Commands

    crown_bashgen_subcommand(NULL, 0, default_level);
    // Flags
    crown_bashgen_options(NULL, 0);
    CROWN_PUTS("  esac\n");

    // case "$prev_word" in
    // "apple")
    //   printf "red\nblue\n"
    //   return 0
    //   ;;
    // esac
    CROWN_PUTS("  if [[ \"${current_word}\" == -* ]]; then\n");
    CROWN_PUTS("  for elem in \"${global_flags[@]}\"; do\n"
               "    if [[ $elem == \"$current_word\"* ]]; then\n"
               "     echo \"$elem\";\n"
               "    fi\n"
               "  done\n");
    CROWN_PUTS("    return 0\n");
    CROWN_PUTS("  fi\n");

    CROWN_PUTS("  for elem in \"${global_commands[@]}\"; do\n"
               "    if [[ $elem == \"$current_word\"* ]]; then\n"
               "     echo \"$elem\";\n"
               "    fi\n"
               "  done\n"
               "}\n"
               "\n");
}

#define CROWN_COMPLETION_BASH (1 << 0)
#define CROWN_COMPLETION_ZSH (1 << 1)
AOCDEF void crown_completion(const CrownEnv *env, size_t envc, int shell) {
    if (shell & CROWN_COMPLETION_ZSH) {
        CROWN_PUTS("#!/usr/bin/env zsh\n\n");
    } else if (shell & CROWN_COMPLETION_BASH) {
        CROWN_PUTS("#!/usr/bin/env bash\n\n");
    }

    // Sets all environment variables to the top
    for (size_t i = 0; i < envc; i++)
        CROWN_PRINTF("%s=\"%s\"\n", env[i].name, env[i].value);

    CROWN_PUTC('\n');

    // Generates all the functions responsible for the completions
    //
    // args.name = "PATH"
    // args.completion = "ls"
    //
    // _PATH() {
    //   ls
    // }
    for (size_t i = 0; i < Program->args->len; i++) {
        CrownArgument args = Program->args->data[i];
        char ARG[CROWN_BUFFER];
        if (args.completion)
            crown_normalize_name(ARG, args.name, CROWN_BUFFER);
        else
            continue;
        const char *arg_comp = args.completion;
        const int arg_comp_len = strlen(arg_comp);

        CROWN_PRINTF("_%s() {\n", args.name);

        for (int j = 0; j < arg_comp_len;) {
            int newline = index_of(arg_comp + j, '\n', arg_comp_len - j);

            if (newline < 0) {
                // no more newlines, print the rest
                CROWN_PRINTF("  %.*s\n", arg_comp_len - j, arg_comp + j);
                break;
            }

            CROWN_PRINTF("  %.*s\n", newline, arg_comp + j);
            j += newline + 1; // +1 to skip '\n'
        }

        CROWN_PUTS("}\n");
    }

    CROWN_PUTC('\n');

    // This amazing strategy has been taken from
    // https://mill-build.org/blog/14-bash-zsh-completion.html

    // TODO: The only difference between the bash and zsh in here is the default_level
    //
    // In bash, words[1] "refers" to the current word, but zsh also counts the program name so
    // it starts at words[2].
    //
    // A simple solution would be store a level variable and increment it. But what if I wanted
    // to support more specific features from zsh or bash that are not compatible? Splitting
    // the function may be a more decent approach on extensibility.
    if (shell & CROWN_COMPLETION_BASH) {
        int default_bash = 1;
        crown_generate_completion(env, envc, default_bash);
    }
    if (shell & CROWN_COMPLETION_ZSH) {
        int default_zsh = 2;
        crown_generate_completion(env, envc, default_zsh);
    }

    if (shell & CROWN_COMPLETION_BASH) {
        CROWN_PUTS(
                "_complete_bash() {\n"
                "  local IFS=$'\\n'\n"
                "  local raw=($(_generate_completions_bash \"$COMP_CWORD\" \"${COMP_WORDS[@]}\"))\n"
                "  local trimmed=()\n"
                "  trimmed+=(\"${raw[@]}\")\n"
                "  \n"
                "  if ((${#raw[@]} == 1)); then\n"
                "    trimmed+=(\"${raw[0]%%:*}\")\n"
                "  fi\n"
                "  \n"
                "  COMPREPLY=( \"${trimmed[@]}\" )\n"
                "}\n");
    }

    if (shell & CROWN_COMPLETION_ZSH) {
        CROWN_PUTS("_complete_zsh() {\n"
                   "  local -a raw trimmed\n"
                   "  local IFS=$'\\n'\n"
                   "  raw=($(_generate_completions_zsh \"$CURRENT\" \"${words[@]}\"))\n"
                   "  \n"
                   "  for d in $raw; do trimmed+=( \"${d%%:*}\" ); done\n"
                   "  if (( ${#raw} == 1 )); then\n"
                   "    trimmed+=( \"${raw[1]}\" )\n"
                   "    raw+=( \"${trimmed[1]}\" )\n"
                   "  fi\n"
                   "  \n"
                   "  compadd -d raw -- $trimmed\n"
                   "}\n");
    }

    CROWN_PUTS("\n");

    if (shell & CROWN_COMPLETION_BASH) {
        CROWN_PRINTF("if [ -n \"${BASH_VERSION:-}\" ]; then\n"
                     "  complete -F _complete_bash %s\n"
                     "fi\n",
                     Program->name);
    }
    if (shell & CROWN_COMPLETION_ZSH) {
        CROWN_PRINTF("if [ -n \"${ZSH_VERSION:-}\" ]; then\n"
                     "  compdef _complete_zsh %s\n"
                     "fi\n",
                     Program->name);
    }
}

AOCDEF void crown_normalize_name(char *buff, const char *str, size_t buff_size) {
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
AOCDEF void crown_print_header(const char *msg) {
    CROWN_PRINTF("%s%s%s", CROWN_HEADER_COLOR, msg, COLOR_RESET);
}

// Helper function
AOCDEF int crown_help_qsort_cmd(const void *a, const void *b) {
    const CrownCommand *CMD_A = (const CrownCommand *)a;
    const CrownCommand *CMD_B = (const CrownCommand *)b;

    return strcmp(CMD_A->name, CMD_B->name);
}

// Helper function
AOCDEF const char *qsort_get_opt(const CrownOption *flag) {
    if (flag->short_opt) return flag->short_opt;
    if (flag->long_opt) return flag->long_opt;
    return "";
}

// Helper function
AOCDEF int crown_help_qsort_opt(const void *a, const void *b) {
    const CrownOption *FLAG_A = (const CrownOption *)a;
    const CrownOption *FLAG_B = (const CrownOption *)b;

    return strcmp(qsort_get_opt(FLAG_A), qsort_get_opt(FLAG_B));
}

// Helper function
AOCDEF bool crown_has_commands(CrownCommand *cmds) {
    CrownCmds *cmd = cmds && cmds->subcmd != NULL ? cmds->subcmd : Program->subcmd;
    if (cmd == NULL) return false;
    if (cmd->len == 0) return false;
#ifndef NDEBUG
    ASSERT(cmd->data != NULL, "No command is defined, but length is %zu\n", cmd->len);
    for (size_t i = 0; i < cmd->len; i++)
        ASSERT(cmd->data[i].name != NULL, "Found a subcommand without name in %s", cmds->name);
#endif
    return true;
}

// Helper function
AOCDEF bool crown_has_options(CrownCommand *cmds) {
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
#define crown_help_command(command)                                                        \
    do {                                                                                   \
        CrownArgument arg = Program->args->data[(command)->args];                          \
        if (arg.name) {                                                                    \
            CROWN_PRINTF("  %s%s%s [%s]\n",                                                \
                         CROWN_COMMAND_COLOR,                                              \
                         (command)->name,                                                  \
                         COLOR_RESET,                                                      \
                         arg.name);                                                        \
        } else {                                                                           \
            CROWN_PRINTF("  %s%s%s\n", CROWN_COMMAND_COLOR, (command)->name, COLOR_RESET); \
        }                                                                                  \
    } while (0)

#define crown_help_usage(command)                                                         \
    do {                                                                                  \
        CrownArgument arg = Program->args->data[(command)->args];                         \
        if (arg.name) {                                                                   \
            CROWN_PRINTF(" %s%s%s [%s]\n",                                                \
                         CROWN_COMMAND_COLOR,                                             \
                         (command)->name,                                                 \
                         COLOR_RESET,                                                     \
                         arg.name);                                                       \
        } else {                                                                          \
            CROWN_PRINTF(" %s%s%s\n", CROWN_COMMAND_COLOR, (command)->name, COLOR_RESET); \
        }                                                                                 \
    } while (0)

// Commands:
//   cmd2 [ARG]
//       Description

// Helper function
AOCDEF void crown_help_commands(CrownCommand *cmds) {
    crown_print_header("Commands:\n");

    const CrownCmds *COMMAND = cmds && cmds->subcmd != NULL ? cmds->subcmd : Program->subcmd;

    for (size_t i = 0; i < COMMAND->len; i++) {
        if (COMMAND->data == NULL) continue;
        CrownCommand cmd = COMMAND->data[i];

        crown_help_command(&cmd);

        if (cmd.desc && cmd.desc[0] != '\0') {
            crown_iprint(cmd.desc, CROWN_INDENTATION);
            CROWN_PUTC('\n');
        }
    }
    CROWN_PUTC('\n');
}

#define crown_help_option(opt)                                                     \
    do {                                                                           \
        const char *SHORT_OPT = (opt)->short_opt;                                  \
        const char *LONG_OPT = (opt)->long_opt;                                    \
        CrownArgument arg = Program->args->data[(opt)->args];                      \
        if (SHORT_OPT && LONG_OPT) {                                               \
            CROWN_PRINTF("  %s%s%s, %s%s%s",                                       \
                         CROWN_COMMAND_COLOR,                                      \
                         SHORT_OPT,                                                \
                         COLOR_RESET,                                              \
                         CROWN_COMMAND_COLOR,                                      \
                         LONG_OPT,                                                 \
                         COLOR_RESET);                                             \
        } else if (LONG_OPT) {                                                     \
            CROWN_PRINTF("  %s%s%s", CROWN_COMMAND_COLOR, LONG_OPT, COLOR_RESET);  \
        } else if (SHORT_OPT) {                                                    \
            CROWN_PRINTF("  %s%s%s", CROWN_COMMAND_COLOR, SHORT_OPT, COLOR_RESET); \
        } else {                                                                   \
            continue;                                                              \
        }                                                                          \
        if (arg.name) CROWN_PRINTF(" [%s]", arg.name);                             \
    } while (0)

// Options:
//   -s, --short [ARG]
//   -l, --long [ARG]
//   -h, --help
//       Description

// Helper function
AOCDEF void crown_help_options(CrownCommand *cmds) {
    crown_print_header("Options:\n");

    const CrownOpts *FLAG = cmds && cmds->flags != NULL ? cmds->flags : Program->flags;

    for (size_t i = 0; i < FLAG->len; i++) {
        if (FLAG->data == NULL) continue;
        CrownOption flags = FLAG->data[i];

        crown_help_option(&flags);

        CROWN_PUTC('\n');
        if (flags.desc && flags.desc[0] != '\0') {
            crown_iprint(flags.desc, CROWN_INDENTATION);
            CROWN_PUTC('\n');
        }
    }
    CROWN_PUTC('\n');
}

// TODO: when no argument is initialized, this segfaults
AOCDEF void crown_help(CrownCommand *null cmd) {
    // NULL in case you want to print the general flags and commands
    if (cmd == NULL) {
        qsort(Program->subcmd->data,
              Program->subcmd->len,
              sizeof(CrownCommand),
              crown_help_qsort_cmd);
        qsort(Program->flags->data, Program->flags->len, sizeof(CrownOption), crown_help_qsort_opt);

        // program name | program description
        CROWN_PRINTF("%s | %s\n\n", Program->name, Program->desc);

        // Usage: program usage
        crown_print_header("Usage:");
        printf(" %s%s%s %s\n\n", CROWN_COMMAND_COLOR, Program->name, COLOR_RESET, Program->usage);
    } else {
        qsort(cmd->subcmd->data, cmd->subcmd->len, sizeof(CrownCommand), crown_help_qsort_cmd);
        qsort(cmd->flags->data, cmd->flags->len, sizeof(CrownOption), crown_help_qsort_opt);

        // command name | command description
        CROWN_PRINTF("%s\n\n", cmd->desc);

        crown_print_header("Usage:");
        crown_help_usage(cmd);
        CROWN_PUTC('\n');
    }
    if (crown_has_commands(cmd)) crown_help_commands(cmd);
    if (crown_has_options(cmd)) crown_help_options(cmd);
}

AOCDEF void crown_dump_args(FILE *fp, CrownArgs *args, size_t indent) {
    crown_indent_completion(indent - 2);
    fprintf(fp, ".args = &(CrownArgs) {\n");
    crown_indent_completion(indent);
    fprintf(fp, ".cap = %zu,\n", args->len);
    crown_indent_completion(indent);
    fprintf(fp, ".len = %zu,\n", args->len);
    crown_indent_completion(indent);
    if (args->len > 0) {
        fprintf(fp, ".data = (CrownArgument[]) {\n");
        range(0, args->len, i) {
            CrownArgument arg = args->data[i];

            crown_indent_completion(indent + 2);
            fprintf(fp, "{\n");

            crown_indent_completion(indent + 4);
            if (arg.name) {
                fprintf(fp, ".name = \"%s\",\n", arg.name);
            } else {
                fprintf(fp, ".name = NULL,\n");
            }

            crown_indent_completion(indent + 4);
            fprintf(fp, ".completion = ");
            if (arg.completion) {
                int completion_len = strlen(arg.completion);
                fputc('"', fp);
                for (int j = 0; j < completion_len; j++) {
                    if (arg.completion[j] == '\n') {
                        fputc('\\', fp);
                        fputc('n', fp);
                        continue;
                    } else if (arg.completion[j] == '"') {
                        fputc('\\', fp);
                        fputc('"', fp);
                        continue;
                    }
                    fputc(arg.completion[j], fp);
                }
                fprintf(fp, "\",\n");
            } else {
                fprintf(fp, "NULL,\n");
            }

            crown_indent_completion(indent + 2);
            fprintf(fp, "},\n"); // CrownArgument
        }
        crown_indent_completion(indent);
        fprintf(fp, "},\n"); // CrownArgument[]
        crown_indent_completion(indent - 2);
        fprintf(fp, "},\n"); // CrownArgs
    } else {
        fprintf(fp, ".data = NULL,\n");
    }
}

AOCDEF void crown_dump_opt(FILE *fp, CrownOpts *opts, size_t indent) {
    crown_indent_completion(indent - 2);
    fprintf(fp, ".flags = &(CrownOpts) {\n");
    crown_indent_completion(indent);
    fprintf(fp, ".cap = %zu,\n", opts->len);
    crown_indent_completion(indent);
    fprintf(fp, ".len = %zu,\n", opts->len);
    crown_indent_completion(indent);
    fprintf(fp, ".data = (CrownOption[]) {\n");
    range(0, opts->len, i) {
        CrownOption opt = opts->data[i];

        crown_indent_completion(indent + 2);
        fprintf(fp, "{\n");

        crown_indent_completion(indent + 4);
        if (opt.short_opt) {
            fprintf(fp, ".short_opt = \"%s\",\n", opt.short_opt);
        } else {
            fprintf(fp, ".short_opt = NULL,\n");
        }

        crown_indent_completion(indent + 4);
        if (opt.long_opt) {
            fprintf(fp, ".long_opt = \"%s\",\n", opt.long_opt);
        } else {
            fprintf(fp, ".long_opt = NULL,\n");
        }

        crown_indent_completion(indent + 4);
        if (opt.desc) {
            fprintf(fp, ".desc = \"%s\",\n", opt.desc);
        } else {
            fprintf(fp, ".desc = NULL,\n");
        }

        crown_indent_completion(indent + 4);
        fprintf(fp, ".args = %d,\n", opt.args);

        crown_indent_completion(indent + 2);
        fprintf(fp, "},\n"); // CrownOption
    }
    crown_indent_completion(indent);
    fprintf(fp, "},\n"); // CrownOption[]
    crown_indent_completion(indent - 2);
    fprintf(fp, "},\n"); // CrownOpts
}

AOCDEF void crown_dump_cmd(FILE *fp, CrownCmds *cmds, size_t indent) {
    crown_indent_completion(indent - 2);
    fprintf(fp, ".subcmd = &(CrownCmds) {\n");
    crown_indent_completion(indent);
    fprintf(fp, ".cap = %zu,\n", cmds->len);
    crown_indent_completion(indent);
    fprintf(fp, ".len = %zu,\n", cmds->len);
    crown_indent_completion(indent);
    fprintf(fp, ".data = (CrownCommand[]) {\n");
    range(0, cmds->len, i) {
        CrownCommand cmd = cmds->data[i];

        crown_indent_completion(indent + 2);
        fprintf(fp, "{\n");

        crown_indent_completion(indent + 4);
        if (cmd.name) {
            fprintf(fp, ".name = \"%s\",\n", cmd.name);
        } else {
            fprintf(fp, ".name = NULL,\n");
        }

        crown_indent_completion(indent + 4);
        if (cmd.desc) {
            fprintf(fp, ".desc = \"%s\",\n", cmd.desc);
        } else {
            fprintf(fp, ".desc = NULL,\n");
        }

        crown_indent_completion(indent + 4);
        fprintf(fp, ".args = %d,\n", cmd.args);

        if (cmd.subcmd && cmd.subcmd->len > 0) {
            crown_dump_cmd(fp, cmd.subcmd, indent + 6);
        }
        if (cmd.flags && cmd.flags->len > 0) {
            crown_dump_opt(fp, cmd.flags, indent + 6);
        }
        crown_indent_completion(indent + 2);
        fprintf(fp, "},\n"); // CrownCommand
    }
    crown_indent_completion(indent);
    fprintf(fp, "},\n"); // CrownCommand[]
    crown_indent_completion(indent - 2);
    fprintf(fp, "},\n"); // CrownCmds
}

AOCDEF void crown_dump(FILE *fp) {
    ASSERT_NONNULL(fp);

    fprintf(fp, "(CrownProgram) {\n");
    crown_indent_completion(2);
    fprintf(fp, ".name = \"%s\",\n", Program->name);
    crown_indent_completion(2);
    fprintf(fp, ".desc = \"%s\",\n", Program->desc);
    crown_indent_completion(2);
    fprintf(fp, ".usage = \"%s\",\n", Program->usage);

    if (Program->subcmd > 0) {
        crown_dump_cmd(fp, Program->subcmd, 4);
    } else {
        crown_indent_completion(2);
        fprintf(fp, ".subcmd = NULL,\n");
    }

    if (Program->flags > 0) {
        crown_dump_opt(fp, Program->flags, 4);
    } else {
        crown_indent_completion(2);
        fprintf(fp, ".flags = NULL,\n");
    }

    if (Program->args > 0) {
        crown_dump_args(fp, Program->args, 4);
    } else {
        crown_indent_completion(2);
        fprintf(fp, ".args = NULL,\n");
    }
    fprintf(fp, "};\n");
}

// CLI Argument Parser

AOCDEF char *crown_getarg(char *argv[], int argc) {
    if (optind >= argc) return NULL;
    return argv[optind++];
}

AOCDEF int crown_getopt(CrownCommand *null cmds, char *argv[], int argc) {
    const char *arg = crown_getarg(argv, argc);
    optcur = (char *)arg;

    if (!arg) return CrownEndOfArgs; // This should never happen...
    if (arg[0] != '-') return CrownNotOpt;

    const CrownOption *opt = cmds && cmds->flags != NULL ? cmds->flags->data : Program->flags->data;
    const size_t len = cmds && cmds->flags != NULL ? cmds->flags->len : Program->flags->len;

    if (opt == NULL) return CrownNotDefined;

    for (size_t i = 0; i < len; i++) {
        const char *long_opt = opt[i].long_opt;
        const char *short_opt = opt[i].short_opt;
        const size_t flag_arg_idx = opt[i].args;
        const char *flag_arg = Program->args && Program->args->data != NULL ?
                                       Program->args->data[flag_arg_idx].name :
                                       NULL;

        if (long_opt != NULL && cstr_eq(arg, long_opt)) {
            if (flag_arg != NULL) {
                optarg = crown_getarg(argv, argc);
                if (optarg == NULL || optarg[0] == '-') return CrownMissingOptarg;
            }
            return i; // Success
        }

        if (short_opt != NULL && cstr_eq(arg, short_opt)) {
            if (flag_arg != NULL) {
                optarg = crown_getarg(argv, argc);
                if (optarg == NULL || optarg[0] == '-') return CrownMissingOptarg;
            }
            return i; // Success
        }
    }
    return CrownNotFound;
}

AOCDEF int crown_getcmd(CrownCommand *null cmds, char *argv[], int argc) {
    const CrownCommand *opt = cmds && cmds->subcmd != NULL ? cmds->subcmd->data :
                                                             Program->subcmd->data;
    const size_t len = cmds && cmds->subcmd != NULL ? cmds->subcmd->len : Program->subcmd->len;

    if (opt == NULL) return CrownNotDefined;

    for (size_t i = 0; i < len; i++) {
        const char *cmd = opt[i].name;
        const size_t cmd_arg_idx = opt[i].args;
        const char *cmd_arg = Program->args && Program->args->data != NULL ?
                                      Program->args->data[cmd_arg_idx].name :
                                      NULL;
        const CrownCommand *cmd_subcmd =
                opt[i].subcmd && opt[i].subcmd->data != NULL ? opt[i].subcmd->data : NULL;

        if (cmd != NULL && cstr_eq(optcur, cmd)) {
            if (cmd_arg != NULL) {
                optarg = crown_getarg(argv, argc);
                if (optarg == NULL || optarg[0] == '-') return CrownMissingOptarg;
            }
            return i; // Success
        }
    }
    return CrownNotDefined;
}

AOCDEF void crown_iprint(const char *msg, int indent) {
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
            CROWN_PUTC('\n');
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
            CROWN_PUTC(' ');
            line_pos++;
        }
    }
    CROWN_PUTC(' ');
}
#endif // AOCLIBS_CROWN

#endif // AOCLIBS_CROWN_H_
