#include <aoclibs/cli/printh.h>
#include <aoclibs/cli/bashgen.h>
#include <aoclibs/common.h>
#include <aoclibs/io/print.h>
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

#define METADATA_DESC "<url> [tag] [title] [note]\n \
                       The url is mandatory"

CLIArgument args[] = {
        [ArgDel] = clarg("del", "id|url|tag", "Either id, url or tag", "find \"$BMARK_DB_DIR\" -type f", ReqArg),
        [ArgEdit] = clarg("field", "field=val metadata", NULL, NULL, ReqArg),
        [ArgIns] = clarg("metadata", "metadata", METADATA_DESC, NULL, ReqArg),
        [ArgNote] = clarg("note", "note", NULL, NULL, ReqArg),
        [ArgPath] = clarg("path", "path", NULL, "ls", ReqArg),
        [ArgTag] = clarg("tag", "tag", NULL, "find \"$BMARK_DB_DIR\" -type f", ReqArg),
        [ArgTitle] = clarg("title", "title", NULL, NULL, ReqArg),
        [ArgURL] = clarg("url", "url", NULL, NULL, ReqArg),
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
        { "-u", "--url",      &args[ArgURL],   "Query url"                        },
};

CLIEnv env[] = {
        { "BMARK_FILE", "${BMARK_FILE:-bookmark.db}" },
        { "BMARK_DB_DIR", "${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}" }
};

// Usage:
int main() {
        bashgen(commands, args, flags, env, ARRAY_LEN(env));
        return 0;
}
