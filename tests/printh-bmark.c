#include "../info/printh.h"

struct ProgramCommands commands[] = {
    {"bulk", NULL, "Edit the database using an EDITOR"},
    {"delete", "ID|URL|TAG", "Delete a bookmark or tag by ID, URL or TAG"},
    {"edit", "field=val URL TAG TITLE NOTES",
     "Edit a bookmark FIELD=VALUE URL TAG TITLE NOTES"},
    {"export", NULL, "Export bookmarks to an HTML file"},
    {"import", NULL, "Import bookmarks from an HTML file"},
    {"insert", "URL TAG TITLE NOTES",
     "Insert a new bookmark URL TAG TITLE NOTES"},
    {"list", "URL TAG TITLE NOTES", "List all bookmarks URL TAG TITLE NOTES"},
    {"setup", NULL, "Manually create a new database"},
    {"version", NULL, "Display current version"}};

struct ProgramFlag flags[] = {
    {NULL, "--note", "note", "Query for NOTE"},
    {NULL, "--tag", "tag", "Query for TAG"},
    {NULL, "--title", "title", "Query for TITLE"},
    {"-u", "--url", "url", "Query for URL"},
    {NULL, "--database", "string", "Use an alternative database"},
    {"-h", "--help", NULL, "Displays this message and exits"},
    {"-r", NULL, NULL, "List only the URL"},
    {"-s", "--strict", NULL, "List will strictly match given query"}};

ProgramInfo program_info = {
    .flag_count = sizeof(flags) / sizeof(flags[0]),
    .command_count = sizeof(commands) / sizeof(commands[0]),
    .prog_name = "bmark",
    .prog_desc = "A simple bookmark manager",
    .prog_usage = "<FLAG> [COMMAND]",
    .commands = commands,
    .flags = flags,
};

int main() {
  printh(program_info);
  return 0;
}
