# Crown

Crown is a full-blown suite for creating CLIs. The goal is to provide the user a way to easily move await the boilerplate while writing a CLI program, without added complexity.

## Features

- Crown has a very simple way to define arguments, environment variables, commands, subcommand and flags.
- Enjoy a completion generator that is easy to use, and don't require you to learn a DSL. It generates Bash and Zsh completions, separately or as a single completion file compatible with both.
- Crown is capable of generating a help message for the program and for each command.
- And mostly importantly, a simple argument parsing that feels like `getopt`, giving you all control over the argument parsing step. To use it, just your C knowledge is enough.

## Example

The current example is roughly enough to introduce all features Crown currently has.

```c
#include <stdio.h>

#define AOCLIBS_IMPLEMENTATION
#include "base.h"

#define AOCLIBS_CROWN
#include "crown.h"

#define AOCLIBS_MATCH
#include "match.h"

bool match_int(int x, int y) {
        return x == y;
}

int main(int argc, char *argv[]) {
        // This step is very important
        //
        // The "Program" variable is just a global pointer. This macro allocate it in the stack.
        //
        // Notably, you can also set some relevant information about your program. Such as:
        crown_init(.name = "bmark",
                   .version = "0.1.0",
                   .desc = "a simple bookmark manager",
                   .usage = "[OPTION] [COMMAND]");

        // ========================================
        // Defining arguments:
        //
        // As an convention, while using Crown, never set an argument for a command that has
        // subcommands. Otherwise, the library won't know what to do.
        //
        // You can have multi-line completion functions
        //
        // echo is, unfortunately, required. This is a tiny tradeoff, so you don't have to learn
        // a DSL.
        //
        // Note, we end the completion with "\n". This is important, and also allows you to write
        // multi-line completion functions for your argument.
        crown_new_arg(path_arg,
                      .name = "PATH", // completion name
                      .completion = "echo \"$(ls)\"\n");

        // If you define an environment variable (done at the ending), and set the same name of it
        // in here, when the user specifies a command or flag that uses it, its argument will update
        // the value of the environment variable.
        crown_new_arg(file_env, "FILE", NULL);

        // These macros will set "path_arg" and "file_env" in the stack, containing their location
        // in Program->args.data.
        // ========================================

        // ========================================
        // New global option
        // CrownNoArg = 0
        crown_new_opt(help_opt,
                      Program,
                      .short_opt = "-h",
                      .long_opt = "--help",
                      .args = CrownNoArg,
                      .desc = NULL);
        // Another one
        crown_new_opt(strict_opt, Program, "-s", "--strict", path_arg, "Use strict rules");
        crown_new_opt(file_opt, Program, "-f", "--file", file_env, "File environment variable");
        // "help_opt", "strict_opt" and "file_opt" are valid CrownOption variables. To get their
        // location within Program->flags.data, use "help_opt_id"...
        // ========================================

        // ========================================
        // New global command
        // CrownCommand also has subcmd and flags, which you should set using the next macros
        //
        // "list_cmd" are valid CrownCommand variables. To get its location within
        // Program->subcmd.data, use "list_cmd_id"
        crown_new_cmd(list_cmd, Program, .name = "list", .args = 0, .desc = "List command");

        // This should be a subcommand for "list":
        //
        // ./program list file
        //
        // subcmd and subopt will automatically track the last command you defined
        // with the "new_cmd" macro.
        crown_new_subcmd(list_file_cmd, .name = "file", .desc = "List all files", .args = 0);
        // This should be a subopt for "list"
        crown_new_subopt(list_strict_opt, "-s", "--symlink", path_arg, "List only symlinks");
        // ========================================

        // ========================================
        // New global command, nothing todo with "list"
        crown_new_cmd(open_cmd, Program, "open", 0, "Open all bookmarks");
        crown_new_subcmd(open_file_cmd, .name = "file", .desc = "Open file", .args = 0);
        // You control if you want a help function or not! You will explictly call:
        // crown_help(open_file_cmd_id) later.
        crown_new_subopt(open_help_opt, "-h", "--help", 0, NULL);
        // For a short-hand: crown_help_flag(open_help_opt)
        // ========================================

        // ========================================
        // >. ./program open file symlink
        crown_new_cmd(open_file_symlink,
                      open_file_cmd,
                      .name = "symlink",
                      .desc = "Open symlink file",
                      .args = path_arg);
        // ========================================

        // Once we have everything set up, if a flag has an argument with the same name
        // as the environment variable, the special handling will be enabled.
        //
        // Currently, this is only used while generating the completion script.
        // You can set these variables, so you can use them in your argument completions.
        CrownEnv envs[] = {
                { file_opt, "FILE", "~/"   },
                { NULL,     "TEST", "true" }, // You can ignore the flag
        };

        // For the parsing mechanism you use it similarly to getopt:
        // while ((opt = getopt(argc, argv, ":h:", NULL)) != -1) {
        //         switch (opt) {
        //         case 'h':
        //                 help();
        //                 return 0;
        //         }

        // But wait, you don't want to pass argv[0].
        const char *prog_name = crown_getarg(argv, argc);

        // We loop till the end of argc
        //
        // Some variables will be available for you:
        //
        // optcur -- current option
        // optind -- option index
        // optarg -- option argument

        while (optind < argc) {
                // crown_parseopt is a macro to crown_getopt. The only thing it does, is pass
                // argv and argc for you.
                //
                // If you pass NULL, it will use the Program->flags. Otherwise, you pass a command
                // here to parse its flags.
                int opt_idx = crown_parseopt(NULL);

                // Match is defined in match.h, as a standalone library. It's handy, but you
                // don't need it. You can achieve the same thing using if/else if/else.
                match(int, opt_idx, match_int) {
                        // Same thing here, NULL is for a help overview from your program
                        // It features the program name, description and usage
                        when(help_opt_id) {
                                printf("%d\n", opt_idx);
                                crown_help(NULL);
                        }
                        else when(strict_opt_id) printf("Enabling strict flag...\n");
                        else when(CrownMissingOptarg) printf("Argument not provided!\n");
                        // CrownNotOpt tells the current id is not a flag. You can either parse it
                        // as an argument, or try to parse a command.
                        else when(CrownNotOpt) {
                                // Because we are still working with Program, we pass NULL here again
                                int opt_idx = crown_parsecmd(NULL);
                                match(int, opt_idx, match_int) {
                                        when(open_cmd_id) printf("Running open command...\n");
                                        else when(list_cmd_id) printf("Running list command...\n");
                                        // This is only returned, if the command requires an argument
                                        else when(CrownMissingOptarg)
                                                printf("Argument not provided!\n");
                                        else printf("Couldn't match any command, "
                                                    "it's probably an argument!\n");
                                }
                        }
                }
        }

        // Crown can generate Bash and Zsh completions. You can do it separatedly, or generate a
        // single completion script compatible with both.
        crown_completion(envs, ARRAY_LEN(envs), CROWN_COMPLETION_BASH | CROWN_COMPLETION_ZSH);

        // Command specific help
        //
        // Crown will make headers bold with underline, and command names bold.
        //
        // You can change this by modifying CROWN_HEADER_COLOR and CROWN_COMMAND_COLOR.
        //
        // The default indentation is 10, and can also be changed in CROWN_INDENTATION.
        crown_help(open_cmd);
        crown_help(list_cmd);

        // Because, the construction of the arguments is done at runtime, a little free is needed:
        crown_deinit();
        return 0;
}
```
