#include "../info/printh.h"

struct PProgramCommands commands[] = {
        { "bulk", NULL, "Edit the database using an EDITOR" },
        { "delete", "ID|URL|TAG",
          "Delete a bookmark or tag by ID, URL or TAG" },
        { "edit", "field=val URL TAG TITLE NOTES",
          "Edit a bookmark FIELD=VALUE URL TAG TITLE NOTES" },
        { "export", NULL, "Export bookmarks to an HTML file" },
        { "import", NULL, "Import bookmarks from an HTML file" },
        { "insert", "URL TAG TITLE NOTES",
          "Insert a new bookmark URL TAG TITLE NOTES" },
        { "list", "URL TAG TITLE NOTES",
          "List all bookmarks URL TAG TITLE NOTES" },
        { "setup", NULL, "Manually create a new database" },
        { "version", NULL, "Display current version" }
};

struct PProgramFlag flags[] = {
        { NULL, "--note", "note", "Query for NOTE" },
        { NULL, "--tag", "tag", "Query for TAG" },
        { NULL, "--title", "title", "Query for TITLE" },
        { "-u", "--url", "url", "Query for URL" },
        { NULL, "--database", "string", "Use an alternative database" },
        { "-h", "--help", NULL, "Displays this message and exits" },
        { "-r", NULL, NULL, "List only the URL" },
        { "-s", "--strict", NULL, "List will strictly match given query" }
};

PProgramInfo program_info = {
        .flagc = sizeof(flags) / sizeof(flags[0]),
        .cmdc = sizeof(commands) / sizeof(commands[0]),
        .name = "bmark",
        .desc = "A simple bookmark manager",
        .usage = "<FLAG> [COMMAND]",
        .commands = commands,
        .flags = flags,
};

int main() {
        printh(program_info);
        return 0;
}
