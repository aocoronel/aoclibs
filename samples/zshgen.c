#define AOCLIBS_IMPLEMENTATION
#define AOCLIBS_STRIP_PREFIX
#define AOCLIBS_ALL
#include "../aoclibs.h"
#include <stddef.h>

#define PROGRAM_NAME "bmark"
#define PROGRAM_DESC "A simple bookmark manager"
#define PROGRAM_USAGE "<OPTION> [COMMAND]"

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

CLIArgument args[] = {
        [ArgDel] = { "id|url|tag",         "Either id, url or tag", FIND_BMARK_DIR                   },
        [ArgEdit] = { "field=val metadata", NULL,                    NULL                             },
        [ArgIns] = { "metadata",           METADATA_DESC,           NULL                             },
        [ArgNote] = { "note",               NULL,                    NULL                             },
        [ArgPath] = { "path",               NULL,                    "ls"                             },
        [ArgTag] = { "tag",                NULL,                    "find \"$BMARK_DB_DIR\" -type f" },
        [ArgTitle] = { "title",              NULL,                    NULL                             },
        [ArgURL] = { "url",                NULL,                    NULL                             },
};

CLICommand commands[] = {
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

CLIOption flags[] = {
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

CLIEnv env[] = {
        { "BMARK_FILE",   "${BMARK_FILE:-bookmark.db}"                    },
        { "BMARK_DB_DIR", "${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}" }
};

CLIProgram prog = aoc_cli_program(commands, args, flags, env);

// Usage:
int main(void) {
        zshgen(prog, env, 2);
        return 0;
}
