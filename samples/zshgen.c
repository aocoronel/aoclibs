#define AOCLIBS_IMPLEMENTATION
#define AOCLIBS_STRIP_PREFIX
#include "../modules/crown.h"
#include <alloca.h>
#include <stddef.h>
#include <stdio.h>

// TODO: This sample contains many legacy code from the previous CLI implementation.
// This will be keept as is, until I update everything to the new implementation.

enum {
        ArgDel,
        ArgEdit,
        ArgIns,
        ArgNote,
        ArgPath,
        ArgTag,
        ArgTitle,
        ArgURL,
};

#define METADATA_DESC \
        "<url> [tag] [title] [note]\n \
                       The url is mandatory"

#define FIND_BMARK_DIR "find \"$BMARK_DB_DIR\" -type f"

CrownArgument args[] = {
        [ArgDel] = { "id|url|tag",         FIND_BMARK_DIR                   },
        [ArgEdit] = { "field=val metadata", NULL                             },
        [ArgIns] = { "metadata",           NULL                             },
        [ArgNote] = { "note",               NULL                             },
        [ArgPath] = { "path",               "ls"                             },
        [ArgTag] = { "tag",                "find \"$BMARK_DB_DIR\" -type f" },
        [ArgTitle] = { "title",              NULL                             },
        [ArgURL] = { "url",                NULL                             },
};

CrownCommand commands[] = {
        // command   argument        description
        { "bulk",    NULL,           "Edit the database using an EDITOR"  },
        { "delete",  &args[ArgDel],  "Delete a bookmark or tag"           },
        { "edit",    &args[ArgEdit], "Edit a bookmark"                    },
        { "export",  NULL,           "Export bookmarks to an HTML file"   },
        { "import",  NULL,           "Import bookmarks from an HTML file" },
        { "insert",  &args[ArgIns],  "Insert a new bookmark"              },
        { "list",    &args[ArgIns],  "List all bookmarks"                 },
        { "setup",   NULL,           "Manually create a new database"     },
        { "version", NULL,           "Display current version"            },
};

CrownOption flags[] = {
        // short & long opts  argument      description
        { "-h", "--help",     NULL,            "Displays this message and exits"      },
        { NULL, "--database", &args[ArgPath],  "Use an alternative database"          },
        { NULL, "--note",     &args[ArgNote],  "Query note"                           },
        { NULL, "--tag",      &args[ArgTag],   "Query tag"                            },
        { NULL, "--title",    &args[ArgTitle], "Query title"                          },
        { "-r", NULL,         NULL,            "List only the url"                    },
        { "-s", "--strict",   NULL,            "List will strictly match given query" },
        { "-u", "--url",      &args[ArgURL],   "Query url"                            },
};

CrownEnv env[] = {
        { "BMARK_FILE",   "${BMARK_FILE:-bookmark.db}"                    },
        { "BMARK_DB_DIR", "${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}" }
};

// CLIProgram prog = aoc_crown_program(commands, args, flags, env);

bool match_int(int x, int y) {
        return x == y;
}

// Usage:
int main(int argc, char *argv[]) {
        // initialize Program variable
        crown_init(.name = "bmark", .version = "0.1.0", .desc = "a simple bookmark manager",
                   .usage = "ded");

        // Add new argument
        // It has a key which is used to be referenced by flags and commands
        crown_new_arg(all_types, .name = "id|url|tag", .completion = FIND_BMARK_DIR);

        // crown_new_opt, subopt, cmd and subcmd will produce a variable allocated in the stack
        // suffixed with _opt or _cmd, that stores the index where it's stored
        //
        // For instance you do:
        // crown_new_opt(help, Program, "-h", NULL, NULL, NULL);
        //
        // This will produce: help_opt
        // In this case it will be 0, and can be found at
        // Program->flags->data[0]
        //
        // This index is only useful, if you are using parseopt or parsecmd

        // ========================================
        // New global opt
        //        varname, parent, short, long, argument, description
        crown_new_opt(help, Program, "-h", "--help", NULL, NULL);
        // Another one
        crown_new_opt(strict, Program, "-s", "--strict", &all_types, "HAHHAHAHAHA");
        // ========================================

        // ========================================
        // New global command
        //         varname, parent, name, argument, description
        // CrownCommand also has subcmd and flags, which you should set using the
        // next macros
        crown_new_cmd(list, Program, "list", &all_types, "List all bookmarks");
        // This should be a subcommand for "list"
        // subcmd and subopt will automatically track the last command you defined
        // with the "new_cmd" macro.
        crown_new_subcmd(list_dog, .name = "dog", .desc = "dogging", .args = &all_types);
        // This should be a subopt for "list"
        crown_new_subopt(list_strict, "-s", "--strict", &all_types, "HAHHAHAHAHA");
        // ========================================

        // ========================================
        // New global command, nothing todo with "list"
        crown_new_cmd(open, Program, "open", NULL, "Open all bookmarks");
        // You can retrieve the command
        // subcommand for "open"
        crown_new_subcmd(open_dog, .name = "dog", .desc = "dogging", .args = &all_types);
        crown_new_subopt(open_strict, "-s", "--strict", &all_types, "HAHHAHAHAHA");
        crown_new_subopt(open_help, "-h", "--help", NULL, NULL);
        // ========================================

        // ========================================
        // >. ./program open dog dog
        crown_new_cmd(open_dog_dog, open_dog, .name = "dog", .desc = "dogging", .args = &all_types);
        // ========================================

        printf("The index that will lead to the open command is %zu\n", open_cmd);
        // For instance you want to argparse the "open" command:

        // Modern CLI libraries will take care of handling everything for you, at what cost?
        // This parseopt and parsecmd tries to be as close to libc getopt as possible, giving
        // you maximum control over what is parsed.
        // In theory, this is supposed to be faster than getopt, since there is no fancy
        // pattern matching. getopt is 688 LOC, while parseopt is 36, and parsecmd is 24.
        // No allocation is made.
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

        crown_zshgen(env, 2);
        crown_bashgen(env, 2);

        // printh(NULL); // Will print the global commands and flags
        // printh(last_cmd - 1); // Prints command specific subcommands and flags
        crown_help(last_cmd);

        // Because, the construction of the arguments is done at runtime, a little free
        // is needed:
        aoc_crown_deinit();

        return 0;
}
