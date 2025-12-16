#include <aoclibs/cli/bashgen.h>
#include <aoclibs/cli/normalize_shell_name.h>
#include <aoclibs/cli/program_info.h>
#include <stddef.h>
#include <stdio.h>

static void bashgen_shebang(void) {
        puts("#!/usr/bin/env bash");
}

static void bashgen_env(const CLIEnv *env, usize envc) {
        for (usize i = 0; i < envc; i++)
                printf("%s=%s\n", env[i].name, env[i].value);
}

static void bashgen_argument(const CLIArgument *args) {
        char ARG[BASHGEN_ARG_BUFFER];
        if (args && args->completion)
                cli_normalize_shell_name(ARG, args->name, BASHGEN_ARG_BUFFER);
        else
                return;
        printf("_%s() {\n  %s\n}\n", ARG, args->completion);
}

static void bashgen_flags(const CLIProgram *prog) {
        putchar(' ');
        for (usize i = 0; i < prog->flagc; i++) {
                if (prog->flags[i].long_opt != NULL)
                        printf(" %s", prog->flags[i].long_opt);
                if (prog->flags[i].short_opt != NULL)
                        printf(" %s", prog->flags[i].short_opt);
        }
}

static void bashgen_commands(const CLIProgram *prog) {
        for (usize i = 0; i < prog->cmdc; i++)
                printf(" %s", prog->commands[i].cmd);
}

static void bashgen_flag_cases(const CLIProgram *prog, const CLIOption *flags) {
        char ARG[BASHGEN_ARG_BUFFER];
        if (flags->args && flags->args->completion)
                cli_normalize_shell_name(ARG, prog->args->name, BASHGEN_ARG_BUFFER);
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
        printf("    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n",
               ARG);
        printf("    return 0\n    ;;\n");
}

static void bashgen_command_cases(const CLIProgram *prog,
                                  const CLICommand *commands) {
        char ARG[BASHGEN_ARG_BUFFER];
        if (commands->args && commands->args->completion)
                cli_normalize_shell_name(ARG, prog->args->name, BASHGEN_ARG_BUFFER);
        else
                return;
        printf("  %s)\n", commands->cmd);
        printf("    mapfile -t COMPREPLY < <(compgen -W \"$(_%s)\" -- \"${cur}\")\n",
               ARG);
        printf("    return 0\n    ;;\n");
}

void _bashgen(const CLIProgram prog, const CLIEnv *env, usize envc) {
        bashgen_shebang();
        bashgen_env(env, envc);
        for (usize i = 0; i < prog.argc; i++) {
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
        for (usize i = 0; i < prog.cmdc; i++) {
                bashgen_command_cases(&prog, &prog.commands[i]);
        }
        for (usize i = 0; i < prog.flagc; i++) {
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
