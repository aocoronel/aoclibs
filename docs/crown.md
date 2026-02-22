# Crown [WIP]

Crown is a full-blown suite for creating CLIs. It includes an argument parser, Bash and Zsh autocompletion generators and help message generator.

```c
#define AOCLIBS_IMPLEMENTATION

#define AOCLIBS_CROWN
#include "crown.h"

#define AOCLIBS_MATCH
#include "match.h"

bool match_int(int x, int y) {
        return x == y;
}

int main(int argc, char *argv[]) {
        // initialize Program variable
        crown_init(.name = "bmark",
                   .version = "0.1.0",
                   .desc = "a simple bookmark manager",
                   .usage = "[OPTION] [COMMAND]");

        crown_new_arg(path, "PATH", "ls");

        // ========================================
        // New global opt
        //        varname, parent, short, long, argument, description
        crown_new_opt(help, Program, "-h", "--help", 0, NULL);
        // Another one
        crown_new_opt(strict, Program, "-s", "--strict", path_arg, "HAHHAHAHAHA");
        // ========================================

        // ========================================
        // New global command
        //         varname, parent, name, argument, description
        // CrownCommand also has subcmd and flags, which you should set using the
        // next macros
        crown_new_cmd(list, Program, "list", 0, "List command");
        // This should be a subcommand for "list"
        // subcmd and subopt will automatically track the last command you defined
        // with the "new_cmd" macro.
        crown_new_subcmd(list_file, .name = "file", .desc = "List all files", .args = path_arg);
        // This should be a subopt for "list"
        crown_new_subopt(list_strict, "-s", "--symlink", path_arg, "List only symlinks");
        // ========================================

        // ========================================
        // New global command, nothing todo with "list"
        crown_new_cmd(open, Program, "open", 0, "Open all bookmarks");
        // You can retrieve the command
        // subcommand for "open"
        crown_new_subcmd(open_file, .name = "file", .desc = "Open file", .args = path_arg);
        // You control if you want a help function or not.
        crown_new_subopt(open_help, "-h", "--help", 0, NULL);
        // ========================================

        // ========================================
        // >. ./program open file symlink
        crown_new_cmd(open_file_symlink,
                      open_file,
                      .name = "symlink",
                      .desc = "Open symlink file",
                      .args = path_arg);
        // ========================================

        while (optind < argc) {
                // open is just an index which will find the command "open"
                // in the Program CrownProgram
                int opt_idx = crown_parseopt(NULL, argv, argc);
                printf("%d\n", opt_idx);
                // Match is defined in base.h.
                // It's handy, but you don't need it.
                match(int, opt_idx, match_int) {
                        when(open_help_opt) printf("handle help\n");
                        else when(open_help_opt) printf("handle strict\n");
                        else when(ArgMissingOptarg) printf("Argument not provided!\n");
                        else when(ArgNotOpt) {
                                // open is just an index which will find the command "open"
                                // in the Program CrownProgram
                                int opt_idx = crown_parsecmd(NULL, argv, argc);
                                printf("getcmd -> %d\n", opt_idx);
                                match(int, opt_idx, match_int) {
                                        when(open_cmd) printf("handle open\n");
                                        else when(list_cmd) printf("handle list\n");
                                        else when(ArgMissingOptarg)
                                                printf("Argument not provided!\n");
                                }
                        }
                }
        }

        // crown_zshgen(); // Zsh completion
        // crown_bashgen(); // Bash completion

        crown_help(NULL);
        crown_help(open);

        // Because, the construction of the arguments is done at runtime, a little free
        // is needed:
        crown_deinit();
        return 0;
}
```
