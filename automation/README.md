# Automation

## Bashgen & Zshgen

A library to generate bash and zsh completion scripts.

Supports:

- Autocomplete commands
- Autocomplete flags
- Autocomplete arguments
- Define variables

### Usage

Define all arguments your program will use:

- Completions set as NULL are discarded.
- Completions are shell commands that are executed for completion.

```c
struct ProgramArguments args[] = {
        { .name = "ID", .completions = NULL }, // Null completions are discarded
        { .name = "TAG", .completions = "ls" }, // When a flag or command uses the TAG argument
                                                // ls will be executed to provide autocompletion
        { .name = "URL", .completions = NULL },
        { .name = "TITLE", .completions = NULL },
        { .name = "NOTE", .completions = NULL },
        { .name = "DB",
          .completions =
                  "sqlite3 \"$BMARK_DB_DIR/$BMARK_FILE\" \"select tag from tags;\"},
};
```

Define all commands your program will use:

- Complex arguments are discarded.
- Description is discarded.

```c
struct ProgramCommands commands[] = {
        // command, argument, description
        { "bulk", NULL, NULL }, // Just bulk is autocompleted
        { "delete", "ID|URL|TAG",
          "Delete a bookmark or tag by ID, URL or TAG" }, // Complex arguments are not supported
        { "edit", "field=val URL TAG TITLE NOTE",
          "Edit a bookmark FIELD=VALUE URL TAG TITLE NOTE" }, // Complex arguments are not supported
        { "export", NULL, "Export bookmarks to an HTML file" }, // Description is discarded
        { "import", NULL, "Import bookmarks from an HTML file" },
        { "insert", "URL TAG TITLE NOTE",
          "Insert a new bookmark URL TAG TITLE NOTE" },
        { "list", "URL TAG TITLE NOTE",
          "List all bookmarks URL TAG TITLE NOTE" },
        { "setup", NULL, "Manually create a new database" },
        { "version", NULL, "Display current version" }
};
```

Define all flags your program will use:

- Complex arguments are discarded.
- Description is discarded.

```c
struct ProgramFlag flags[] = {
        { NULL, "--note", "NOTE", "Query for NOTE" }, // long flag set
        { "-T", "--tag", "TAG", "Query for TAG" }, // TAG argument will be autocompleted
        { NULL, "--title", "TITLE", "Query for TITLE" }, // Description is discarded
        { "-u", "--url", "URL", "Query for URL" }, // short and long flags set
        { NULL, "--database", "DB", "Use an alternative database" },
        { "-h", "--help", NULL, "Displays this message and exits" },
        { "-r", NULL, NULL, "List only the URL" },
        { "-s", "--strict", NULL, "List will strictly match given query" }
};
```

Provide program metadata:

```c
ProgramInfo program_info = {
        .flagc = sizeof(flags) / sizeof(flags[0]),
        .cmdc = sizeof(commands) / sizeof(commands[0]),
        .name = "bmark",
        .desc = "A simple bookmark manager",
        .usage = "<FLAG> [COMMAND]",
        .commands = commands,
        .flags = flags,
};
```

You may define variables to be used during autocompletion:

```c
struct ProgramEnv envs[] = {
        { "BMARK_FILE", "${BMARK_FILE:-bookmark.db}" },
        { "BMARK_DB_DIR", "${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}" }
};
```

Finally, glue everything together:

```c
CompletionInfo completion_info = {
        .info = &program_info,
        .argc = sizeof(args) / sizeof(args[0]),
        .envc = sizeof(envs) / sizeof(envs[0]),
        .args = args,
        .envs = envs,
};
```

When everything is set up, generating the completion is easy:

```c
int main() {
        generate_bash_completion(&completion_info);
        generate_zsh_completion(&completion_info);
        return 0;
}
```

### Results

Expected result for `bashgen`:

```bash
#!/usr/bin/env bash
BMARK_FILE=${BMARK_FILE:-bookmark.db}
BMARK_DB_DIR=${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}
_TAG() {
  ls
}
_DB() {
  sqlite3 "$BMARK_DB_DIR/$BMARK_FILE" "select tag from tags;" | tr "\n" " "
}
_bmark() {
  COMPREPLY=()
  cur="${COMP_WORDS[COMP_CWORD]}"
  prev="${COMP_WORDS[COMP_CWORD - 1]}"
  case "${cur}" in
  -*)
    mapfile -t COMPREPLY < <(compgen -W " --note --tag -T --title --url -u --database --help -h -r --strict -s" -- "${cur}")
    return 0
    ;;
  esac
  case "${prev}" in
  -T | --tag)
    mapfile -t COMPREPLY < <(compgen -W "$(_TAG)" -- "${cur}")
    return 0
    ;;
  --database)
    mapfile -t COMPREPLY < <(compgen -W "$(_DB)" -- "${cur}")
    return 0
    ;;
  esac
  mapfile -t COMPREPLY < <(compgen -W " bulk delete edit export import insert list setup version" -- "${cur}")
  return 0
}
complete -F _bmark bmark%
```

Expected result for `zshgen`:

```bash
#compdef bmark

BMARK_FILE=${BMARK_FILE:-bookmark.db}
BMARK_DB_DIR=${BMARK_DB_DIR:-$HOME/.local/share/bookmarks}
_bmark() {
  local -a subcommands

  subcommands=(
    "bulk:Edit the database using an EDITOR"
    "delete:Delete a bookmark or tag by ID, URL or TAG"
    "edit:Edit a bookmark FIELD=VALUE URL TAG TITLE NOTE"
    "export:Export bookmarks to an HTML file"
    "import:Import bookmarks from an HTML file"
    "insert:Insert a new bookmark URL TAG TITLE NOTE"
    "list:List all bookmarks URL TAG TITLE NOTE"
    "setup:Manually create a new database"
    "version:Display current version"
  )

  _arguments -C \
    '1:command:->subcmds' \
    '--note=[Query for NOTE]:NOTE' \
    '--tag=[Query for TAG]:TAG:_bmark_get_TAG' \
    '-T[Query for TAG]:TAG:_bmark_get_TAG' \
    '--title=[Query for TITLE]:TITLE' \
    '--url=[Query for URL]:URL' \
    '-u[Query for URL]:URL' \
    '--database=[Use an alternative database]:DB:_bmark_get_DB' \
    '--help=[Displays this message and exits]' \
    '-h[Displays this message and exits]' \
    '-r[List only the URL]' \
    '--strict=[List will strictly match given query]' \
    '-s[List will strictly match given query]' \
    '*::args:->command_args'

  case $state in
    subcmds)
      _describe 'command' subcommands
      return
      ;;
    command_args)
      case $words[1] in
        -T | --tag)
          _arguments \
            '*:TAG:_bmark_get_TAG' \
          return
          ;;
        --database)
          _arguments \
            '*:DB:_bmark_get_DB' \
          return
          ;;
      esac
      ;;
  esac
}

_bmark_get_DB() {
  local results
  results=(${(f)"$(ls 2>/dev/null)"})
  compadd -Q -a results
}

_bmark_get_TAG() {
  local results
  results=(${(f)"$(sqlite3 "$BMARK_DB_DIR/$BMARK_FILE" "select tag from tags;" 2>/dev/null)"})
  compadd -Q -a results
}

compdef _bmark bmark
```
