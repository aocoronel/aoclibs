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

## History

Originally Crown was a feature rich, with support for comamnds, subcommands, sub options and a lot of nesting for hierarchical organization. Over time, I realized that trying to add all of these features was too complicated to maintain and extend, so I walked back, and turned Crown into a option only library. The cost, is that you no longer easily nesting of commands and flags, however all parsing was linear before, and it's powered by a hashmap now, which guarantees a excellent performance no matter how many flags you have.

The main reason I did this shift, was because I was fascinated on how nice it is to use the C compiler and change it's behavior. Instead of hierarchical structure, it's organic, and the order doesn't matter much, not only to mention it has thousands of flags and still very fast. Why should I ever bother making so generic, if I can specialize the library into only options and make it great?

Another thing that made me switch was because the old parsing was fully manual, not a single piece of automation, but giving you the structure to do so. The current one does parsing for you, however the parsing is partially implemented by Crown, and it's up to you to make the parser. By that, I mean each option has to provide a function that will be responsible for parsing the argument, and you can make it as much complicated as you want. This is the way it is, because many modern CLI parsing libraries commit a very bad decision of fully automating the parsing step for you, and now they have to implement a parsing mechanism for each single possible user need, incredibly bloating the library. The previous implementation of Crown was 1000+ lines of C, the current one is less than that, and gives you all freedom to parse arguments the way you want, for the various edge cases you could have.
