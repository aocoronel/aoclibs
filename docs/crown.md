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
        .arg = { "bool", NULL },
      },
      { "u", "uint", "Set int",
        .fn = crown_collect_uint, .var = &my_uint,
        .arg = { "uint", NULL },
      },
      { "p", "path", "Set path",
        .fn = crown_collect_char_env, .var = &path,
        .arg = { "FILEPATH", NULL },
        // If an environment is set, and has the same name as the argument, the completion script
        // will always keep it updated, so other completions can use it
        .env = { "FILEPATH", "~/" },
      },
      { 0 },
    };
    // clang-format on

    // Always skip the program name
    const char *progname = crown_getarg(argc, argv);

    size_t opt_len = 0;
    crown_init(options, &opt_len);
    while (OPTIND < argc) {
        const char *err = crown_parse(options, argc, argv);
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

    crown_help(options, opt_len, "bmark", "minimalistic bookmark manager", "[OPTIONS]");

    crown_completion(options, opt_len, "bmark", NULL, CROWN_COMPLETION_BASH | CROWN_COMPLETION_ZSH);

    return 0;
}
```
