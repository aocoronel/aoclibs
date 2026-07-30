#ifndef CROWN_H_
#define CROWN_H_

#include "base.h"
#include "colors.h"
#include "hmap.h"
#include <sys/ioctl.h>
#include <unistd.h>

/*
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

History

Originally Crown was a feature rich, with support for comamnds, subcommands, sub options and a lot of nesting for hierarchical organization. Over time, I realized that trying to add all of these features was too complicated to maintain and extend, so I walked back, and turned Crown into a option only library. The cost, is that you no longer easily nesting of commands and flags, however all parsing was linear before, and it's powered by a hashmap now, which guarantees a excellent performance no matter how many flags you have.

The main reason I did this shift, was because I was fascinated on how nice it is to use the C compiler and change it's behavior. Instead of hierarchical structure, it's organic, and the order doesn't matter much, not only to mention it has thousands of flags and still very fast. Why should I ever bother making so generic, if I can specialize the library into only options and make it great?

Another thing that made me switch was because the old parsing was fully manual, not a single piece of automation, but giving you the structure to do so. The current one does parsing for you, however the parsing is partially implemented by Crown, and it's up to you to make the parser. By that, I mean each option has to provide a function that will be responsible for parsing the argument, and you can make it as much complicated as you want. This is the way it is, because many modern CLI parsing libraries commit a very bad decision of fully automating the parsing step for you, and now they have to implement a parsing mechanism for each single possible user need, incredibly bloating the library. The previous implementation of Crown was 1000+ lines of C, the current one is less than that, and gives you all freedom to parse arguments the way you want, for the various edge cases you could have.
*/

#define CROWN_BUFFER 256 // used by crown_normalize_name
#define CROWN_INDENTATION 6 // Indentation by spaces

// Separator used by flags to indicate value
#ifndef CROWN_SEPARATOR
#define CROWN_SEPARATOR ':'
#endif

// Coloring
#define CROWN_HEADER_COLOR COLOR_BOLD_UNDERLINE
#define CROWN_COMMAND_COLOR COLOR_BOLD

#ifndef CROWN_OUTPUT
#define CROWN_OUTPUT stdout
#endif

#define $crown_printf(...) fprintf(CROWN_OUTPUT, __VA_ARGS__)
#define $crown_puts(string) fputs((string), CROWN_OUTPUT)
#define $crown_putc(c) fputc(c, CROWN_OUTPUT)

typedef struct Crown_Hashmap Crown_Hashmap;
typedef struct Crown_Option Crown_Option;

typedef const char *(*Crown_Fn)(int argc, char **argv, Crown_Option *opt);

struct Crown_Option {
	const char *short_opt, *long_opt, *desc;

	Crown_Fn fn;
	void *var;

	struct {
		const char *name;
		const char *completion;
		bool required;
	} arg;

	const char *env;
};

struct Crown_Hashmap {
	HASHMAP(size_t);
};

extern Crown_Hashmap CROWN_HASHMAP;

// Current index on argv, same behavior as getopt's optind
extern int OPTIND;
// Next argument, same behavior as getopt's optarg
extern char *OPTARG;
// Current argument, same behavior as getopt's optopt
extern char *OPTOPT;
extern char *OPTCURR;

extern size_t CROWN_OPTION_LEN;
extern const Crown_Option *CROWN_OPTION;

// === Standard API

#define $crown_set(opts)                             \
	do {                                             \
		CROWN_OPTION = ((const Crown_Option *)opts); \
		CROWN_OPTION_LEN = $array_len(opts) - 1;     \
	} while (0)

#define CROWN_NOT_OPT ((const char *)-1)
// Parses command-line arguments
//
// CROWN_NOT_OPT :: you may use this to parse positional arguments
//
// Example:
//   $crown_set(options);
//   crown_init();
//   while (OPTIND < argc) {
//       const char *err = crown_parse(argc, argv);
//       if (!err) continue;
//       if (err != CROWN_NOT_OPT) {
//           printf("error: %s: %s\n", err, OPTOPT);
//           continue;
//       }
//       // parse positional arguments manually
//   }
//   crown_dinit();
AOCDEF const char *crown_parse(int argc, char *argv[]);

#define CROWN_COMPLETION_BASH (1 << 0)
#define CROWN_COMPLETION_ZSH (1 << 1)
// Generate autocompletion script
//
// The "shell" accepts either CROWN_COMPLETION_BASH, CROWN_COMPLETION_ZSH, or both.
//
// TODO: The "default_completion" is the command that is used when no completion is available
AOCDEF void crown_completion(
    const char *restrict progname, const char *restrict null default_completion, int shell);

// Prints help message.
AOCDEF void
crown_help(const char *restrict progname, const char *restrict desc, const char *restrict usage);

// Dump the hashmap in a stack-based format, so the user can use it to create a constant hashmap, and
// use Crown without allocating
AOCDEF void crown_compile(FILE *fd);

// Init the hashmap
AOCDEF const char *null crown_init(void);

// Deinit the hashmap
AOCDEF void crown_deinit(void);

// Get next argument from argv.
AOCDEF char *crown_getarg(int argc, char *argv[]);

// === Extra API

#ifdef CROWN_EXTRA
// Reads separator value, if no argument is given, try getenv()
AOCDEF const char *crown_collect_char_env(int argc, char **argv, Crown_Option *opt);

// Reads separator value, or next argument
AOCDEF const char *crown_collect_int(int argc, char **argv, Crown_Option *opt);

// Reads separator value, or next argument
AOCDEF const char *crown_collect_uint(int argc, char **argv, Crown_Option *opt);

// Reads separator value, or invert
AOCDEF const char *crown_collect_bool(int argc, char **argv, Crown_Option *opt);
#endif // CROWN_EXTRA

// === Helper API

// Reads separator
//
// NULL :: No argument found
AOCDEF const char *null crown_get_separator_value(const char *null arg);

// Reads separator and next argument
//
// NULL :: No argument found
AOCDEF const char *null crown_get_value(int argc, char **argv, const char *null arg);

// Checks if the next argument is an option
AOCDEF bool crown_is_next_option(void);

// Prints indented message of given "indent".
//
// If the size of the terminal cannot be obtained, fallback to 80 columns.
AOCDEF void crown_iprint(const char *msg, int indent);

// Normalizes a given "str" to a valid shell function name, by replacing it to non-alphanumeric
// characters with underscores.
//
// May truncate to "buff_size".
AOCDEF void crown_normalize_name(char *restrict buff, const char *restrict str, size_t buff_size);

// Print the completion code for the options
AOCDEF void crown_generate_options(int indent);

// Print the special completion algorithm for environment variables
//
// When an argument and a environment variable set in share the same name, this function will
// add a lookup for loop, which will update the environment variable at completion
//
// Example:
// 1. Flag "-d" has argument and env named "MY_DIR".
// 2. Flag "-f" has argument completion that uses "MY_DIR" as base directory to search for files
// 3. User pass "-d:~/"
// 4. User pass "-f=[TAB]", will autocomplete with files from "MY_DIR"
AOCDEF void crown_generate_env_vars(int idx);

// Print shell specific entry point
AOCDEF void
crown_generate_completion(int idx, const char *null default_completion, int default_level);

AOCDEF const char *crown_parse_shortopt(int argc, char *argv[]);
AOCDEF const char *crown_parse_longopt(int argc, char *argv[]);

// Print help options section
AOCDEF void crown_help_options(void);

// Function to dump the hashmap
AOCDEF const char *crown_hash_dump(void *x, size_t idx);

#ifdef CROWN

#include "cstr.h"
#include "io.h"

int OPTIND = 0;
char *OPTARG = NULL;
char *OPTOPT = NULL;
char *OPTCURR = NULL;

Crown_Hashmap CROWN_HASHMAP = { { 0 } };

const Crown_Option *CROWN_OPTION = NULL;
size_t CROWN_OPTION_LEN = 0;

void crown_generate_options(int indent) {
	$range(0, CROWN_OPTION_LEN, i) {
		const Crown_Option opt = CROWN_OPTION[i];

		const char *sopt = opt.short_opt;
		const char *lopt = opt.long_opt;
		const char *arg_name = opt.arg.name;

		$assert(lopt || sopt, "Option doesn't has a short or long flag");

		if (!arg_name) continue;

		char ARG[CROWN_BUFFER];
		crown_normalize_name(ARG, arg_name, CROWN_BUFFER);

		fputw(CROWN_OUTPUT, indent);
		if (lopt && sopt)
			$crown_printf("  -%s|--%s)\n", sopt, lopt);
		else if (lopt)
			$crown_printf("  --%s)\n", lopt);
		else if (sopt)
			$crown_printf("  -%s)\n", sopt);
		fputw(CROWN_OUTPUT, indent);
		$crown_printf("    echo \"$(_%s)\"\n", ARG);
		fputw(CROWN_OUTPUT, indent);
		$crown_puts("    return 0\n");
		fputw(CROWN_OUTPUT, indent);
		$crown_puts("    ;;\n");
	}
}

void crown_generate_env_vars(int idx) {
	$range(0, CROWN_OPTION_LEN, i) {
		const Crown_Option opt = CROWN_OPTION[i];
		const char *env = opt.env;
		const char *sopt = opt.short_opt;
		const char *lopt = opt.long_opt;
		if (!env) continue;

		const char *arg_name = opt.arg.name;
		$assert(
		    arg_name != NULL && cstr_eq(arg_name, env),
		    "argument name isn't equal to environment name");

		$crown_printf("  for ((i = %d; i < ${#words[@]}; i++)); do\n", idx);
		if (sopt && lopt) {
			$crown_printf(
			    "    if [[ \"${words[i]}\" == \"-%s\" ]] || [[ \"${words[i]}\" == \"--%s\" ]] && ((i + 1 < ${#words[@]})); then\n",
			    sopt, lopt);
		} else if (sopt) {
			$crown_printf(
			    "    if [[ \"${words[i]}\" == \"--%s\" ]] && ((i + 1 < ${#words[@]})); then\n",
			    lopt);
		} else if (lopt) {
			$crown_printf(
			    "    if [[ \"${words[i]}\" == \"-%s\" ]] && ((i + 1 < ${#words[@]})); then\n",
			    sopt);
		}
		$crown_printf("       %s=\"${words[i + 1]}\"\n", env);
		$crown_puts("       break\n");
		$crown_puts("    fi\n");
		$crown_puts("  done\n\n");
	}
}

void crown_generate_completion(int idx, const char *default_completion, int default_level) {
	if (default_level == 1)
		$crown_puts("_generate_completions_bash() {\n");
	else if (default_level == 2)
		$crown_puts("_generate_completions_zsh() {\n");

	// For reference:
	// "  local prev_word=${words[idx - 1]}\n"
	// "  local level=${#words[@]}\n"
	$crown_puts(
	    "  local idx=\"$1\"; shift\n"
	    "  local words=( \"$@\" )\n"
	    "  local current_word=${words[idx]}\n"
	    "\n");

	crown_generate_env_vars(idx);

	// Argument completion
	$crown_printf("  case \"$current_word\" in\n");
	crown_generate_options(0);
	$crown_puts("  esac\n");

	$crown_puts(
	    "  if [[ \"${current_word}\" == -* ]]; then\n"
	    "    for elem in \"${global_flags[@]}\"; do\n"
	    "      if [[ $elem == \"$current_word\"* ]]; then\n"
	    "       echo \"$elem\";\n"
	    "      fi\n"
	    "    done\n"
	    "    return 0\n"
	    "  fi\n");
	if (default_completion) {
		$crown_printf("  %s;\n", default_completion);
	}
	$crown_puts(
	    "  return 0\n"
	    "}\n\n");
}

void crown_completion(const char *progname, const char *default_completion, int shell) {
	$assert_nonnull(progname);
	if (shell & CROWN_COMPLETION_ZSH) {
		$crown_puts("#!/usr/bin/env zsh\n\n");
	} else if (shell & CROWN_COMPLETION_BASH) {
		$crown_puts("#!/usr/bin/env bash\n\n");
	}

	// Sets all environment variables to the top
	$range(0, CROWN_OPTION_LEN, i) {
		const Crown_Option opt = CROWN_OPTION[i];
		if (opt.env) $crown_printf("%s=\"${%s:-}\"\n", opt.env, opt.env);
	}
	$crown_putc('\n');

	// TODO: Having issues to print files+directories in a portable way that works for both bash
	// and zsh
	const char *completion = default_completion;
	if (!completion) {
		// completion = "echo \"$(_complete_file)\"";
	}

	$crown_puts("global_flags=(\n");
	$range(0, CROWN_OPTION_LEN, i) {
		const Crown_Option opt = CROWN_OPTION[i];
		const char *sopt = opt.short_opt;
		const char *lopt = opt.long_opt;
		const char *desc = opt.desc;

		if (lopt) {
			if (desc) {
				$crown_printf("  \"--%s: %s\"\n", lopt, desc);
			} else {
				$crown_printf("  \"--%s\"\n", lopt);
			}
		}
		if (sopt) {
			if (desc) {
				$crown_printf("  \"-%s: %s\"\n", sopt, desc);
			} else {
				$crown_printf("  \"-%s\"\n", sopt);
			}
		}
	}
	$crown_puts(")\n\n");

	// Generates all the functions responsible for the completions
	//
	// args.name = "PATH"
	// args.completion = "ls"
	//
	// _PATH() {
	//   ls
	// }
	$range(0, CROWN_OPTION_LEN, i) {
		const Crown_Option opt = CROWN_OPTION[i];
		const char *name = opt.arg.name;
		const char *completion = opt.arg.completion;

		if (!name || !completion) continue;

		char ARG[CROWN_BUFFER];
		crown_normalize_name(ARG, name, CROWN_BUFFER);

		const int completion_len = strlen(completion);

		$crown_printf("_%s() {\n", name);

		for (int j = 0; j < completion_len;) {
			int newline = index_of(completion + j, '\n', completion_len - j);

			if (newline < 0) {
				// no more newlines, print the rest
				$crown_printf("  %.*s\n", completion_len - j, completion + j);
				break;
			}

			$crown_printf("  %.*s\n", newline, completion + j);
			j += newline + 1; // +1 to skip '\n'
		}

		$crown_puts("}\n");
	}

	$crown_putc('\n');

	// This amazing strategy has been taken from
	// https://mill-build.org/blog/14-bash-zsh-completion.html

	// The only difference between the Bash and Zsh in here is the default_level and starting idx
	//
	// Bash "words[1]" refers to the current word, but Zsh also counts the program name so
	// it starts at words[2].
	//
	// Bash is zero indexed, Zsh is 1 indexed (like Lua)
	if (shell & CROWN_COMPLETION_BASH) {
		int default_bash = 1;
		crown_generate_completion(0, completion, default_bash);
	}
	if (shell & CROWN_COMPLETION_ZSH) {
		int default_zsh = 2;
		crown_generate_completion(1, completion, default_zsh);
	}
	// $crown_puts(
	//     "_complete_file() {\n"
	//     "  if [ -n \"${BASH_VERSION:-}\" ]; then\n"
	//     "      local IFS=$'\n'\n"
	//     "      COMPREPLY=($(compgen -f -- \"$DIR\" 2>/dev/null))\n"
	//     "  elif [ -n \"${ZSH_VERSION:-}\" ]; then\n"
	//     "      _files -g \"$DIR*\"\n"
	//     "  fi\n"
	//     "  return 0\n"
	//     "}\n");

	if (shell & CROWN_COMPLETION_BASH) {
		$crown_puts(
		    "_complete_bash() {\n"
		    "  local IFS=$'\\n'\n"
		    "  local raw=($(_generate_completions_bash \"$COMP_CWORD\" \"${COMP_WORDS[@]}\"))\n"
		    "  local trimmed=()\n"
		    "  trimmed+=(\"${raw[@]}\")\n"
		    "  \n"
		    "  if ((${#raw[@]} == 1)); then\n"
		    "    trimmed+=(\"${raw[0]%%:*}\")\n"
		    "  fi\n"
		    "  \n"
		    "  COMPREPLY=( \"${trimmed[@]}\" )\n"
		    "}\n");
	}

	if (shell & CROWN_COMPLETION_ZSH) {
		$crown_puts(
		    "_complete_zsh() {\n"
		    "  local -a raw trimmed\n"
		    "  local IFS=$'\\n'\n"
		    "  raw=($(_generate_completions_zsh \"$CURRENT\" \"${words[@]}\"))\n"
		    "  \n"
		    "  for d in $raw; do trimmed+=( \"${d%%:*}\" ); done\n"
		    "  if (( ${#raw} == 1 )); then\n"
		    "    trimmed+=( \"${raw[1]}\" )\n"
		    "    raw+=( \"${trimmed[1]}\" )\n"
		    "  fi\n"
		    "  \n"
		    "  compadd -d raw -- $trimmed\n"
		    "}\n");
	}

	$crown_puts("\n");

	if (shell & CROWN_COMPLETION_BASH) {
		$crown_printf(
		    "if [ -n \"${BASH_VERSION:-}\" ]; then\n"
		    "  complete -F _complete_bash %s\n"
		    "fi\n",
		    progname);
	}
	if (shell & CROWN_COMPLETION_ZSH) {
		$crown_printf(
		    "if [ -n \"${ZSH_VERSION:-}\" ]; then\n"
		    "  compdef _complete_zsh %s\n"
		    "fi\n",
		    progname);
	}
}

void crown_normalize_name(char *buff, const char *str, size_t buff_size) {
	$assert_nonnull(buff);
	$assert_nonnull(str);
	if (!buff || !str || buff_size == 0) {
		if (buff && buff_size > 0) buff[0] = '\0';
		return;
	}

	size_t i = 0;

	for (; *str && i < buff_size; str++) {
		if (isalnum(*str) || *str == '_') {
			buff[i++] = *str;
		} else {
			buff[i++] = '_';
		}
	}

	if (i < buff_size)
		buff[i] = '\0';
	else
		buff[buff_size - 1] = '\0';

	if (i > 0 && isdigit(buff[0])) {
		memmove(buff + 1, buff, i);
		buff[0] = '_';
	}
}

// CLI Argument Parser

AOCDEF char *crown_getarg(int argc, char *argv[]) {
	if (OPTIND >= argc) return NULL;
	return argv[OPTIND++];
}

const char *crown_parse_shortopt(int argc, char *argv[]) {
	char short_opt[2] = { 0 };
	short_opt[0] = *OPTCURR;

	size_t *i = hmap_get_value(&CROWN_HASHMAP, cstr_to_slice(short_opt));
	if (!i) return "option not found";
	Crown_Option opt = CROWN_OPTION[*i];
	$assert(opt.fn, "function not set for flag '%s'", opt.short_opt);

	OPTCURR++;
	OPTARG = *(argv + 1);
	Crown_Fn fn = opt.fn;
	return fn(argc, argv, &opt);
}

const char *crown_parse_longopt(int argc, char *argv[]) {
	char *flag;
	char long_opt[256] = { 0 };

	char *idx = (char *)memchr(OPTCURR, CROWN_SEPARATOR, strlen(OPTCURR));
	if (idx) {
		size_t len = idx - OPTCURR;
		if (len > 255) return "flag is too long";
		memcpy(long_opt, OPTCURR, len);
		flag = long_opt;
	} else {
		flag = OPTCURR;
	}

	size_t *i = hmap_get_value(&CROWN_HASHMAP, cstr_to_slice(flag));
	if (!i) return "option not found";
	Crown_Option opt = CROWN_OPTION[*i];
	$assert(opt.fn, "function not set for flag '%s'", opt.long_opt);
	OPTCURR = idx;
	OPTARG = *(argv + 1);
	Crown_Fn fn = opt.fn;
	return fn(argc, argv, &opt);
}

const char *crown_parse(int argc, char *argv[]) {
	OPTOPT = crown_getarg(argc, argv);
	OPTCURR = OPTOPT;
	$assert(OPTCURR, "OPTIND > argc");

	if (*OPTCURR != '-') return CROWN_NOT_OPT;
	OPTCURR++;

	if (*OPTCURR == '-') {
		OPTCURR++;
		return crown_parse_longopt(argc, argv);
	}

	while (*OPTCURR && *OPTCURR != CROWN_SEPARATOR) {
		const char *err = crown_parse_shortopt(argc, argv);
		if (err) return err;
	}
	return NULL;
}

void crown_help_options() {
	$range(0, CROWN_OPTION_LEN, i) {
		const Crown_Option opt = CROWN_OPTION[i];

		const char *desc = opt.desc;
		const char *arg = opt.arg.name;
		const char *env = opt.env;
		const char *sopt = opt.short_opt;
		const char *lopt = opt.long_opt;

		if (sopt && lopt) {
			$crown_printf(
			    "  %s%s%s, %s%s%s", CROWN_COMMAND_COLOR, sopt, COLOR_RESET, CROWN_COMMAND_COLOR,
			    lopt, COLOR_RESET);
		} else if (lopt) {
			$crown_printf("  %s%s%s", CROWN_COMMAND_COLOR, lopt, COLOR_RESET);
		} else if (sopt) {
			$crown_printf("  %s%s%s", CROWN_COMMAND_COLOR, sopt, COLOR_RESET);
		} else {
			continue;
		}

		if (arg) {
			if (env && cstr_eq(env, arg)) {
				$crown_printf(" $%s", arg);
			} else {
				if (opt.arg.required) {
					$crown_printf(" <%s>", arg);
				} else {
					$crown_printf(" [%s]", arg);
				}
			}
		}
		$crown_putc('\n');

		if (desc && desc[0] != '\0') {
			crown_iprint(desc, CROWN_INDENTATION);
			$crown_putc('\n');
		}
	}
	$crown_putc('\n');
}

void crown_help(const char *progname, const char *desc, const char *usage) {
	// program | description
	$crown_printf("%s | %s\n\n", progname, desc);
	// Usage: program usage
	$crown_printf("%s%s%s", CROWN_HEADER_COLOR, "Usage:", COLOR_RESET);
	printf(" %s%s%s %s\n\n", CROWN_COMMAND_COLOR, progname, COLOR_RESET, usage);
	// Options:
	//   -s, --short [ARG]
	//   -l, --long [ARG]
	//   -h, --help
	//       Description
	$crown_printf("%s%s%s\n", CROWN_HEADER_COLOR, "Options:", COLOR_RESET);
	crown_help_options();
}

void crown_iprint(const char *msg, int indent) {
	struct winsize w;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1 || w.ws_col == 0) {
		w.ws_col = 80;
	}
	const int WIDTH = w.ws_col;

	$crown_printf("%-*s", indent, "");
	int line_pos = indent;

	const char *START = msg;
	const char *END = msg;

	while (*END) {
		if (*END == '\n') {
			$crown_putc('\n');
			$crown_printf("%*s", indent, "");
			line_pos = indent;
			END++;
			continue;
		}
		while (isspace((unsigned char)*END))
			END++;

		if (*END == ' ') break;

		START = END;
		while (*END && !isspace((unsigned char)*END))
			END++;

		int word_len = END - START;

		if (line_pos + word_len > WIDTH && line_pos > indent) {
			$crown_printf("\n%*s", indent, "");
			line_pos = indent;
		}

		$crown_printf("%.*s", word_len, START);
		line_pos += word_len;

		if (*END && line_pos < WIDTH) {
			$crown_putc(' ');
			line_pos++;
		}
	}
	$crown_putc(' ');
}

const char *crown_get_separator_value(const char *arg) {
	if (!arg) return NULL;
	if (*arg == CROWN_SEPARATOR) return ++arg;
	return NULL;
}

bool crown_is_next_option(void) {
	if (!OPTARG) return false;
	if (OPTARG[0] == '-') {
		return true;
	}
	return false;
}

const char *crown_get_value(int argc, char **argv, const char *arg) {
	const char *value undefined;
	value = crown_get_separator_value(arg);
	if (!value) {
		if (crown_is_next_option()) return NULL;
		value = crown_getarg(argc, argv);
		if (!value) return NULL;
	}
	return value;
}

#ifdef CROWN_EXTRA
const char *crown_collect_char_env(int argc, char **argv, Crown_Option *opt) {
	const char **v = (const char **)opt->var;

	const char *value = crown_get_separator_value(OPTCURR);
	if (value) {
		*v = value;
	} else {
		$assert(opt->env);
		const char *env = getenv(opt->env);
		if (env) {
			*v = env;
		}
	}

	// printf("[%d] %s\n", OPTIND, *v);

	return NULL;
}

const char *crown_collect_int(int argc, char **argv, Crown_Option *opt) {
	long long *v = (long long *)opt->var;

	const char *value = crown_get_value(argc, argv, OPTCURR);
	if (!value) return "missing argument";
	*v = cstr_to_llong(value, *v);

	// printf("[%d] %lld\n", OPTIND, *v);

	return NULL;
}

const char *crown_collect_uint(int argc, char **argv, Crown_Option *opt) {
	unsigned long long *v = (unsigned long long *)opt->var;

	const char *value = crown_get_value(argc, argv, OPTCURR);
	if (!value) return "missing argument";
	*v = cstr_to_ullong(value, *v);

	// printf("[%d] %llu\n", OPTIND, *v);

	return NULL;
}

const char *crown_collect_bool(int argc, char **argv, Crown_Option *opt) {
	bool *v = (bool *)opt->var;

	const char *value = crown_get_separator_value(OPTCURR);
	if (value) {
		*v = cstr_to_bool(value, *v);
	} else {
		*v = !*v;
	}

	// printf("[%d] %s\n", OPTIND, *v ? "true" : "false");

	return NULL;
}
#endif // CROWN_EXTRA

const char *null crown_init(void) {
	if (!hmap_init(&CROWN_HASHMAP, 256)) return "out of memory";
	size_t i = 0;
	const Crown_Option *opts = (const Crown_Option *)CROWN_OPTION;
	for (; opts && opts->short_opt || opts->long_opt; opts++) {
		const Crown_Option opt = *opts;

		da_insert(&CROWN_HASHMAP, i);

		if (opt.short_opt) {
			Hash_Entry_Tmpl *entry = hmap_prepare(&CROWN_HASHMAP, cstr_to_slice(opt.short_opt));
			if (entry) {
				$assert(entry->hash == 0, "the option '%s' was already inserted", opt.short_opt);
				entry->value_idx = CROWN_HASHMAP.len - 1;
			} else {
				return "out of memory";
			}
		}
		if (opt.long_opt) {
			Hash_Entry_Tmpl *entry = hmap_prepare(&CROWN_HASHMAP, cstr_to_slice(opt.long_opt));
			if (entry) {
				$assert(entry->hash == 0, "the option '%s' was already inserted", opt.long_opt);
				entry->value_idx = CROWN_HASHMAP.len - 1;
			} else {
				return "out of memory";
			}
		}
		i++;
	}
	return NULL;
}

const char *crown_hash_dump(void *x, size_t idx) {
	static char buff[32] = { 0 };
	size_t *y = (size_t *)x;

	memset(buff, 0, 32);
	snprintf(buff, 32, "%zu", y[idx]);

	return buff;
}

void crown_deinit(void) {
	hmap_free(&CROWN_HASHMAP);
}

void crown_compile(FILE *fd) {
	hmap_dump(fd, &CROWN_HASHMAP, "Crown_Hashmap", "size_t", crown_hash_dump);
}
#endif // CROWN

#endif // CROWN_H_
