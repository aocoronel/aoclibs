#include <aoclibs/cli/bashgen.h>
#include <aoclibs/cli/program_info.h>
#include <stddef.h>
#include <stdio.h>

static void bashgen_shebang(void) {
        printf("#!/usr/bin/env bash\n");
}

static void bashgen_env(const CLIEnv *env, usize envc) {
        for (int i = 0; i < envc; i++) {
                printf("%s=%s\n", env[i].name, env[i].value);
        }
}

static void bashgen_argument(const CLIProgram *prog) {
        for (int i = 0; i < prog->argc; i++) {
                if (prog->args[i].completion == NULL) continue;
                printf("_%s() {\n", prog->args[i].id);
                printf("  %s\n", prog->args[i].completion);
                printf("}\n");
        }
}

void _bashgen(const CLIProgram prog, const CLIEnv *env, usize envc) {
        bashgen_shebang();
        bashgen_env(env, envc);
        bashgen_argument(&prog);

        // Main function
        printf("_%s() {\n", prog.name);
        printf("  COMPREPLY=()\n");
        printf("  cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");
        printf("  prev=\"${COMP_WORDS[COMP_CWORD - 1]}\"\n");

        // Autocomplete flags
        printf("  case \"${cur}\" in\n");
        printf("  -*)\n");
        printf("    mapfile -t COMPREPLY < <(compgen -W \"");
        for (int i = 0; i < prog.flagc; i++) {
                const char *SHORT_FLAG = prog.flags[i].short_opt;
                const char *LONG_FLAG = prog.flags[i].long_opt;

                if (LONG_FLAG != NULL) {
                        printf(" %s", LONG_FLAG);
                }
                if (SHORT_FLAG != NULL) {
                        printf(" %s", SHORT_FLAG);
                }
        }
        printf("\" -- \"${cur}\"%c\n    return 0\n    ;;\n  esac\n", ')');

        // Autocomplete arguments from flags and commands
        printf("  case \"${prev}\" in\n");
        int j = 0;

        for (int i = 0; i < prog.cmdc; i++) {
                if (prog.commands[i].cmd && prog.commands[i].args &&
                    prog.commands[i].args->id &&
                    prog.commands[i].args->completion) {
                        const char *COMMAND = prog.commands[i].cmd;
                        const char *ARG_NAME = prog.commands[i].args->id;

                        printf("  %s%c\n", COMMAND, ')');
                        printf("    mapfile -t COMPREPLY < <(compgen -W \"$\(_%s%c\" -- \"${cur}\"%c\n",
                               ARG_NAME, ')', ')');
                        printf("    return 0\n");
                        printf("    ;;\n");
                } else {
                        j++;
                        // If command does not have argument with completions, fallback
                        if (j == prog.cmdc) {
                                printf("  \"\"%c\n", ')');
                                printf("    return 1\n    ;;\n");
                                break;
                        }
                }
        }
        for (int i = 0; i < prog.flagc; i++) {
                if (prog.flags[i].args && prog.flags[i].args->id &&
                    prog.flags[i].args->completion) {
                        const char *SHORT_FLAG = prog.flags[i].short_opt;
                        const char *LONG_FLAG = prog.flags[i].long_opt;
                        const char *ARG_NAME = prog.flags[i].args->id;

                        if (LONG_FLAG && SHORT_FLAG) {
                                printf("  %s | %s%c\n", SHORT_FLAG, LONG_FLAG,
                                       ')');
                        } else if (LONG_FLAG) {
                                printf("  %s%c\n", LONG_FLAG, ')');
                        } else if (SHORT_FLAG) {
                                printf("  %s%c\n", SHORT_FLAG, ')');
                        }
                        printf("    mapfile -t COMPREPLY < <(compgen -W \"$\(_%s%c\" -- \"${cur}\"%c\n",
                               ARG_NAME, ')', ')');
                        printf("    return 0\n");
                        printf("    ;;\n");
                } else {
                        j++;
                        // If flags does not have argument with completions, fallback
                        if (j == prog.flagc) {
                                printf("  \"\"%c\n", ')');
                                printf("    return 1\n    ;;\n");
                                break;
                        }
                }
        }
        // End main function
        printf("  esac\n");

        // Autocomplete commands
        printf("  mapfile -t COMPREPLY < <(compgen -W \"");
        for (int i = 0; i < prog.cmdc; i++) {
                printf(" %s", prog.commands[i].cmd);
        }
        printf("\" -- \"${cur}\"%c\n  return 0\n}\n", ')');

        // Assign function to program
        printf("complete -F _%s %s", prog.name, prog.name);
}
