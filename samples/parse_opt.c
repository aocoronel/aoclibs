#define AOCLIBS_IMPLEMENTATION
#define AOCLIBS_ALL
#define AOCLIBS_STRIP_PREFIX
#include "../aoclibs.h"

#include <stdio.h>
#include <stddef.h>

#define PROGRAM_NAME "bmark"
#define PROGRAM_DESC "A simple bookmark manager"
#define PROGRAM_USAGE "[OPTION...] [COMMAND] [ARG...]"

#define METADATA_DESC \
        "<url> [tag] [title] [note]\n \
                       The url is mandatory"

#define FIND_BMARK_DIR "find \"$BMARK_DB_DIR\" -type f"

// Short-hand to access specific argument in cli_args
#define ARG(name) (&cli_args[Arg##name])

// Enum, Argument Name, Description, Completion
#define ARG_LIST(X)                                                   \
        X(Del, "id|url|tag", "Either id, url or tag", FIND_BMARK_DIR) \
        X(Edit, "field=val metadata", NULL, NULL)                     \
        X(Ins, "metadata", METADATA_DESC, NULL)                       \
        X(Note, "note", NULL, NULL)                                   \
        X(Path, "path", NULL, "ls")                                   \
        X(Tag, "tag", NULL, FIND_BMARK_DIR)                           \
        X(Title, "title", NULL, NULL)                                 \
        X(URL, "url", NULL, NULL)

// Enum, Short Opt, Long Opt, Argument, Description
#define FLAG_LIST(X)                                                              \
        X(Database, NULL, "--database", ARG(Path), "Use an alternative database") \
        X(Help, "-h", "--help", NULL, "Displays this message and exits")          \
        X(Note, NULL, "--note", ARG(Note), "Query note")                          \
        X(Raw, "-r", NULL, NULL, "List only the url")                             \
        X(Strict, "-s", "--strict", NULL, "List will strictly match given query") \
        X(Tag, NULL, "--tag", ARG(Tag), "Query tag")                              \
        X(Title, NULL, "--title", ARG(Title), "Query title")                      \
        X(URL, "-u", "--url", ARG(URL), "Query url")

enum {
#define X(name, ...) Arg##name,
        ARG_LIST(X)
#undef X
                _ArgCount
};

enum {
#define X(name, ...) Opt##name,
        FLAG_LIST(X)
#undef X
                _OptCount,
};

CLIArgument cli_args[_ArgCount] = {
#define X(name, arg, desc, comp) [Arg##name] = { arg, desc, comp },
        ARG_LIST(X)
#undef X
};

CLIOption cli_flags[_OptCount] = {
#define X(name, s, l, arg, desc) [Opt##name] = { s, l, arg, desc },
        FLAG_LIST(X)
#undef X
};

CLICommand cli_cmds[] = {
        // command   argument    description
        { "bulk",    NULL,      "Edit the database using an EDITOR"  },
        { "delete",  ARG(Del),  "Delete a bookmark or tag"           },
        { "edit",    ARG(Edit), "Edit a bookmark"                    },
        { "export",  NULL,      "Export bookmarks to an HTML file"   },
        { "import",  NULL,      "Import bookmarks from an HTML file" },
        { "insert",  ARG(Ins),  "Insert a new bookmark"              },
        { "list",    ARG(Ins),  "List all bookmarks"                 },
        { "setup",   NULL,      "Manually create a new database"     },
        { "version", NULL,      "Display current version"            },
};

CLIEnv env[] = {
        { "BMARK_FILE",   "${BMARK_FILE:-bookmark.db}"                    },
        { "BMARK_DB_DIR", "${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}" }
};

CLIProgram prog = aoc_cli_program(cli_cmds, cli_args, cli_flags, env);

// Usage:
int main(int argc, char *argv[]) {
        while (optind < argc) {
                int opt_idx = getopt(argv, argc, &prog);
                switch (opt_idx) {
                case OptTag:
                        printf("tag is %s\n", optarg);
                        break;
                case OptTitle:
                        printf("title is: %s\n", optarg);
                        break;
                case OptHelp:
                        printh(prog);
                        break;
                case ArgMissingOptarg:
                        printf("Argument not provided!\n");
                        break;
                }
        }
        return 0;
}
