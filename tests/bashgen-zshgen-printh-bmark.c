#include <stddef.h>
#include <stdio.h>
#include "../automation/bashgen.h"
#include "../automation/zshgen.h"
#include "../info/printh.h"

struct ProgramArguments args[] = {
        { .name = "ID", .completions = NULL },
        { .name = "TAG", .completions = "ls" },
        { .name = "URL", .completions = NULL },
        { .name = "TITLE", .completions = NULL },
        { .name = "NOTE", .completions = NULL },
        { .name = "DB",
          .completions =
                  "sqlite3 \"$BMARK_DB_DIR/$BMARK_FILE\" \"select tag from tags;\" | tr \"\\n\" \" \"" },
};

struct ProgramCommands commands[] = {
        { "bulk", NULL, "Edit the database using an EDITOR" },
        { "delete", "ID|URL|TAG",
          "Delete a bookmark or tag by ID, URL or TAG" },
        { "edit", "field=val URL TAG TITLE NOTE",
          "Edit a bookmark FIELD=VALUE URL TAG TITLE NOTE" },
        { "export", NULL, "Export bookmarks to an HTML file" },
        { "import", NULL, "Import bookmarks from an HTML file" },
        { "insert", "URL TAG TITLE NOTE",
          "Insert a new bookmark URL TAG TITLE NOTE" },
        { "list", "URL TAG TITLE NOTE",
          "List all bookmarks URL TAG TITLE NOTE" },
        { "setup", NULL, "Manually create a new database" },
        { "version", NULL, "Display current version" }
};

struct ProgramFlag flags[] = {
        { NULL, "--note", "NOTE", "Query for NOTE" },
        { "-T", "--tag", "TAG", "Query for TAG" },
        { NULL, "--title", "TITLE", "Query for TITLE" },
        { "-u", "--url", "URL", "Query for URL" },
        { NULL, "--database", "DB", "Use an alternative database" },
        { "-h", "--help", NULL, "Displays this message and exits" },
        { "-r", NULL, NULL, "List only the URL" },
        { "-s", "--strict", NULL, "List will strictly match given query" }
};

ProgramInfo program_info = {
        .flagc = sizeof(flags) / sizeof(flags[0]),
        .cmdc = sizeof(commands) / sizeof(commands[0]),
        .name = "bmark",
        .desc = "A simple bookmark manager",
        .usage = "<FLAG> [COMMAND]",
        .commands = commands,
        .flags = flags,
};

struct ProgramEnv envs[] = {
        { "BMARK_FILE", "${BMARK_FILE:-bookmark.db}" },
        { "BMARK_DB_DIR", "${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}" }
};

CompletionInfo completion_info = {
        .info = &program_info,
        .argc = sizeof(args) / sizeof(args[0]),
        .envc = sizeof(envs) / sizeof(envs[0]),
        .args = args,
        .envs = envs,
};

int main() {
        printf("Help message:\n\n");
        printh(program_info);
        printf("\n\nZsh Completions:\n\n");
        generate_zsh_completion(&completion_info);
        printf("\n\nBash Completions:\n\n");
        generate_bash_completion(&completion_info);
        return 0;
}
