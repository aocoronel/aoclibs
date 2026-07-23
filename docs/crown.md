# Crown

Crown is a option parsing library, but also provides you a help message generation and autocompletion script generation for Bash and Zsh. Crown uses a hashmap for option parsing, which can be initialized at runtime, or prepared before-hand and made available at compile-time. Initializing the options is simple and centralized.

Crown also provides you nice helpers to build your own parsers, for the types you need.

## Example

The current example is roughly enough to introduce all features Crown currently has.

```c
#define CROWN
// CROWN_EXTRA contains the collect functions, see below
#define CROWN_EXTRA
#include "crown.h"

int main(int argc, char *argv[]) {
    bool my_bool = false;
    const char *path = NULL;
    unsigned long long my_uint = 0ULL;

    // clang-format off
    const Crown_Option options[] = {
      // short, long, description
      { "s", "short", "Short message",
        // The "fn" is always required, the "var" is only required, depending on which function
        // you use
        .fn = crown_collect_bool, .var = &my_bool,
        // If a flag accepts an argument, you can describe it. You can do whatever
        // This is only used in the help message and completion script
        // The required field is only used in the help message, to help the user understand how to use
        // the program
        .arg = { "bool", NULL, .required = false },
      },
      { "u", "uint", "Set int",
        .fn = crown_collect_uint, .var = &my_uint,
        .arg = { "uint", NULL, .required = true },
      },
      { "f", "file", "Set path",
        .fn = crown_collect_char_env, .var = &path,
        .arg = { "filepath", NULL, .required = false },
        // Environment used in the completion script, and may be used in the callback
        // If the env is equal to the argument name, the script will collect the env value, and
        // all other completions that depends on it will have it updated
        // Say that one flag gets file completions from $DIR, and another flag sets $DIR, if $DIR is
        // previously set, and the file flag is used, the completions will use the set path, for example
        .env = "filepath",
      },
      { 0 },
    };
    // clang-format on
    $crown_set(options);

    // Always skip the program name
    const char *progname = crown_getarg(argc, argv);

    crown_init();

    // The user may pass something like:
    // ./program -psu:355 --short:false
    while (OPTIND < argc) {
        const char *err = crown_parse(argc, argv);
        if (!err) continue;
        if (err != CROWN_NOT_OPT) {
            printf("error: %s: %s\n", err, OPTOPT);
            continue;
        }
        // parse positional arguments manually
    }

    // This will dump the hashmap used to parse the arguments.
    // If you have a build system, you can use this to initialize the hashmap without crown_init(),
    // skipping all allocations
    crown_compile(stdout);

    crown_deinit();

    crown_help("bmark", "minimalistic bookmark manager", "[OPTIONS]");

    crown_completion("bmark", NULL, CROWN_COMPLETION_BASH | CROWN_COMPLETION_ZSH);

    return 0;
}
```
