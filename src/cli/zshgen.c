#include <aoclibs/cli/zshgen.h>
#include <stdio.h>

void cli_normalize_shell_name(char *out, const char *in, size_t max_len);

static void zshgen_print_arg_autocomplete(const CLIProgram *prog, const CLIArgument *args) {
        char ARG[ZSHGEN_ARG_BUFFER] = { 0 };
        if (args)
                cli_normalize_shell_name(ARG, args->name, ZSHGEN_ARG_BUFFER);
        else
                return;
        const char *COMPLETIONS = args->completion;
        if (COMPLETIONS != NULL) {
                printf("_%s_get_%s() {\n", prog->name, ARG);
                printf("  local results\n");
                printf("  results=(${(f)\"$(%s 2>/dev/null)\"})\n",
                       COMPLETIONS);
                printf("  compadd -Q -a results\n");
                printf("}\n\n");
        }
}

static void zshgen_print_flag_arg(const CLIProgram *prog,
                                  const CLIOption *flag) {
        char ARG[ZSHGEN_ARG_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                cli_normalize_shell_name(ARG, flag->args->name, ZSHGEN_ARG_BUFFER);
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

static void zshgen_print_command_case(const CLIProgram *prog,
                                      const CLICommand *cmd) {
        char ARG[ZSHGEN_ARG_BUFFER];
        if (cmd->args)
                cli_normalize_shell_name(ARG, cmd->args->name, ZSHGEN_ARG_BUFFER);
        else
                return;
        const char *COMP = (cmd->args) ? cmd->args->completion : NULL;
        if (!COMP) return;

        printf("        %s)\n", cmd->cmd);
        printf("          _arguments \\\n");
        printf("            '*:%s:_%s_get_%s' \\\n", ARG, prog->name, ARG);
        printf("          ;;\n");
}

static void zshgen_print_flag_case(const CLIProgram *prog,
                                   const CLIOption *flag) {
        char ARG[ZSHGEN_ARG_BUFFER];
        const char *SHORT_FLAG = flag->short_opt;
        const char *LONG_FLAG = flag->long_opt;
        if (flag->args)
                cli_normalize_shell_name(ARG, flag->args->name, ZSHGEN_ARG_BUFFER);
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

void _zshgen(const CLIProgram prog, const CLIEnv *env, usize envc) {
        // Header
        printf("#compdef %s\n\n", prog.name);

        // Environment defaults
        for (int i = 0; i < envc; i++) {
                printf("%s=%s\n", env[i].name, env[i].value);
        }

        // Main function
        printf("_%s() {\n", prog.name);
        printf("  local -a subcommands\n\n");

        // Define Subcommands
        printf("  subcommands=(\n");
        for (int i = 0; i < prog.cmdc; i++) {
                printf("    \"%s:%s\"\n", prog.commands[i].cmd,
                       prog.commands[i].desc);
        }
        printf("  )\n\n");

        // Define arguments
        printf("  _arguments -C \\\n");
        printf("    '1:command:->subcmds' \\\n");
        for (int i = 0; i < prog.flagc; i++) {
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
        for (int i = 0; i < prog.cmdc; i++) {
                zshgen_print_command_case(&prog, &prog.commands[i]);
        }
        // Autocomplete arguments from flags
        for (int i = 0; i < prog.flagc; i++) {
                zshgen_print_flag_case(&prog, &prog.flags[i]);
        }
        printf("      esac\n");
        printf("      ;;\n");
        printf("  esac\n");
        printf("}\n\n");

        // Define helper functions to autocomplete arguments
        for (int i = 0; i < prog.argc; i++) {
                zshgen_print_arg_autocomplete(&prog, &prog.args[i]);
        }

        // Assign function to program
        printf("compdef _%s %s\n", prog.name, prog.name);
}
