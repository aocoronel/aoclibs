# Crown [WIP]

Crown is a full-blown suite for creating CLIs. It includes an argument parser, Bash and Zsh autocompletion generators and help message generator.

```c
int main(int argc, char *argv[]) {
        // initialize Program variable
        crown_init(.name = "bmark", .version = "0.1.0", .desc = "a simple bookmark manager",
                   .usage = "ded");

        // Add new argument
        // It has a key which is used to be referenced by flags and commands
        CrownArgument Crown_Argument[] = {
              {.name = "PATH", .completion = "ls" },
        }

        // ========================================
        // New global opt
        //        varname, parent, short, long, argument, description
        crown_new_opt(help, Program, "-h", "--help", NULL, NULL);
        // Another one
        crown_new_opt(strict, Program, "-s", "--strict", "PATH", "HAHHAHAHAHA");
        // ========================================

        // ========================================
        // New global command
        //         varname, parent, name, argument, description
        // CrownCommand also has subcmd and flags, which you should set using the
        // next macros
        crown_new_cmd(list, Program, "list", NULL, "List command");
        // This should be a subcommand for "list"
        // subcmd and subopt will automatically track the last command you defined
        // with the "new_cmd" macro.
        crown_new_subcmd(list_file, .name = "file", .desc = "List all files", .args = "PATH");
        // This should be a subopt for "list"
        crown_new_subopt(list_strict, "-s", "--symlink", "PATH", "List only symlinks");
        // ========================================

        // ========================================
        // New global command, nothing todo with "list"
        crown_new_cmd(open, Program, "open", NULL, "Open all bookmarks");
        // You can retrieve the command
        // subcommand for "open"
        crown_new_subcmd(open_file, .name = "file", .desc = "Open file", NULL);
        // You control if you want a help function or not.
        crown_new_subopt(open_help, "-h", "--help", NULL, NULL);
        // ========================================

        // ========================================
        // >. ./program open file symlink
        crown_new_cmd(open_file_symlink, open_file, .name = "symlink", .desc = "Open symlink file", .args = "PATH");
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
                        else when(open_strict_opt) printf("handle strict\n");
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

        crown_zshgen(); // Zsh completion
        crown_bashgen(); // Bash completion

        // printh(NULL); // Will print the global commands and flags
        // printh(last_cmd - 1); // Prints command specific subcommands and flags
        crown_help(open);

        // Because, the construction of the arguments is done at runtime, a little free
        // is needed:
        aoc_crown_deinit();
```
