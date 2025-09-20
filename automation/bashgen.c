#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

struct BProgramArguments {
        const char *name;
        const char *completions;
};

struct PProgramFlag {
        const char *short_flag;
        const char *long_flag;
        const char *argument;
        const char *desc;
};

struct PProgramCommands {
        const char *cmd;
        const char *argument;
        const char *cmd_desc;
};

typedef struct {
        int flagc;
        int cmdc;
        const char *name;
        const char *desc;
        const char *usage;
        struct PProgramCommands *commands;
        struct PProgramFlag *flags;
} PProgramInfo;

typedef struct {
        PProgramInfo *info;
        struct BProgramArguments *args;
} CommandRegistry;

void generate_bash_completion(const CommandRegistry *info) {
        printf("#!/usr/bin/env bash\n");
        printf("_%s() {\n", info->info->name);
        printf("  COMPREPLY=()\n");
        printf("  cur=\"${COMP_WORDS[COMP_CWORD]}\"\n");
        printf("  prev=\"${COMP_WORDS[COMP_CWORD - 1]}\"\n");

        printf("  case \"${cur}\" in\n");
        printf("  -)\n");
        printf("    mapfile -t COMPREPLY < <(compgen -W \"");
        for (int i = 0; i < info->info->flagc; i++) {
                const char *short_flag = info->info->flags[i].short_flag;
                const char *long_flag = info->info->flags[i].long_flag;

                if (long_flag) {
                        printf(" %s", long_flag);
                } else if (short_flag) {
                        printf(" %s", short_flag);
                }
        }
        printf("\"%c\n    return 0\n    ;;\n  esac\n", ')');

        // char *arg_copy = NULL;
        // strcpy(arg_copy, info->info->commands->argument);
        //
        // char *token;
        // int index = 0;
        // char *tokens[10];
        //
        // char *equal = strrchr(info->info->commands->argument, '=');
        // char *pipe = strchr(info->info->commands->argument, '|');
        // if (pipe) {
        //         token = strtok(arg_copy, "|");
        //         while (token != NULL) {
        //                 tokens[index++] = token;
        //                 token = strtok(NULL, "|");
        //         }
        //
        //         for (int i = 0; i < index; i++) {
        //                 if (tokens[i] == info->info->commands->argument) {
        //                         printf("%s\\)\n", info->info->commands->cmd);
        //                         printf("    mapfile -t COMPREPLY < <\(\"%s\" -- \"${cur}\"\\)",
        //                                info->info->commands->argument);
        //                 }
        //         }
        // }

        printf("  mapfile -t COMPREPLY < <(compgen -W \"");
        for (int i = 0; i < info->info->cmdc; i++) {
                const char *bash_commands = info->info->commands[i].cmd;

                printf(" %s", bash_commands);
        }
        printf("\"%c\n  return 0\n}\n", ')');
        printf("complete -F _%s %s", info->info->name, info->info->name);
}

struct BProgramArguments args[] = {
        { .name = "ID", .completions = NULL },
        { .name = "TAG", .completions = NULL },
        { .name = "URL", .completions = NULL },
        { .name = "TITLE", .completions = NULL },
        { .name = "NOTE", .completions = NULL },
        { .name = "DB",
          .completions =
                  "sqlite3 '$BMARK_DB_DIR/$BMARK_FILE' 'select tag from tags;' | tr '\n' ' '" },
};

struct PProgramCommands commands[] = {
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

struct PProgramFlag flags[] = {
        { NULL, "--note", "NOTE", "Query for NOTE" },
        { NULL, "--tag", "TAG", "Query for TAG" },
        { NULL, "--title", "TITLE", "Query for TITLE" },
        { "-u", "--url", "URL", "Query for URL" },
        { NULL, "--database", "DB", "Use an alternative database" },
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

CommandRegistry program_registry = {
        .info = &program_info,
        .args = args,
};

int main() {
        generate_bash_completion(&program_registry);
        return 0;
}
