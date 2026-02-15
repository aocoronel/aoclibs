# Modules

## Crown [WIP]

Crown is a full-blown suite for creating CLIs. It includes an argument parser, Bash and Zsh autocompletion generators and help message generator.

```c
int main(int argc, char *argv[]) {
        // initialize Program variable
        crown_init(.name = "bmark", .version = "0.1.0", .desc = "a simple bookmark manager",
                   .usage = "ded");

        // Add new argument
        // It has a key which is used to be referenced by flags and commands
        CrownArgument Crown_Argument[] = {
              {.name = "PATH", .completion = "ls" },
        }

        // ========================================
        // New global opt
        //        varname, parent, short, long, argument, description
        crown_new_opt(help, Program, "-h", "--help", NULL, NULL);
        // Another one
        crown_new_opt(strict, Program, "-s", "--strict", "PATH", "HAHHAHAHAHA");
        // ========================================

        // ========================================
        // New global command
        //         varname, parent, name, argument, description
        // CrownCommand also has subcmd and flags, which you should set using the
        // next macros
        crown_new_cmd(list, Program, "list", NULL, "List command");
        // This should be a subcommand for "list"
        // subcmd and subopt will automatically track the last command you defined
        // with the "new_cmd" macro.
        crown_new_subcmd(list_file, .name = "file", .desc = "List all files", .args = "PATH");
        // This should be a subopt for "list"
        crown_new_subopt(list_strict, "-s", "--symlink", "PATH", "List only symlinks");
        // ========================================

        // ========================================
        // New global command, nothing todo with "list"
        crown_new_cmd(open, Program, "open", NULL, "Open all bookmarks");
        // You can retrieve the command
        // subcommand for "open"
        crown_new_subcmd(open_file, .name = "file", .desc = "Open file", NULL);
        // You control if you want a help function or not.
        crown_new_subopt(open_help, "-h", "--help", NULL, NULL);
        // ========================================

        // ========================================
        // >. ./program open file symlink
        crown_new_cmd(open_file_symlink, open_file, .name = "symlink", .desc = "Open symlink file", .args = "PATH");
        // ========================================

        while (optind < argc) {
                // open is just an index which will find the command "open"
                // in the Program CrownProgram
                int opt_idx = crown_parseopt(NULL, argv, argc);
                printf("%d\n", opt_idx);
                // Match is defined in base.h.
                // It's handy, but you don't need it.
                match(int, opt_idx, match_int) {
                        when(open_help_opt) printf("handle help\n");
                        else when(open_strict_opt) printf("handle strict\n");
                        else when(ArgMissingOptarg) printf("Argument not provided!\n");
                        else when(ArgNotOpt) {
                                // open is just an index which will find the command "open"
                                // in the Program CrownProgram
                                int opt_idx = crown_parsecmd(NULL, argv, argc);
                                printf("getcmd -> %d\n", opt_idx);
                                match(int, opt_idx, match_int) {
                                        when(open_cmd) printf("handle open\n");
                                        else when(list_cmd) printf("handle list\n");
                                        else when(ArgMissingOptarg)
                                                printf("Argument not provided!\n");
                                }
                        }
                }
        }

        crown_zshgen(); // Zsh completion
        crown_bashgen(); // Bash completion

        // printh(NULL); // Will print the global commands and flags
        // printh(last_cmd - 1); // Prints command specific subcommands and flags
        crown_help(open);

        // Because, the construction of the arguments is done at runtime, a little free
        // is needed:
        aoc_crown_deinit();
```

## Morph

Morph is a simple and deterministic code generator.

The goal is to generate code using C, and writing the instructions as close as C.

It doesn't require an external executable to generate code. The only thing you need is a C compiler.

Currently Morph has these features:

1. Consistent indentation defined by MORPH_INDENTATION.

2. Generate defines, which includes the result of a given expression or function:

This code will create a DEFINE named LOOP_RESULT, which has the value of 30.

```c
int loop_10_times(int x) {
     for (int i = 0; i < 10; i++) x++;
     return x;
}

int myvar = comp_int(LOOP_RESULT, loop_10_times(20));
```

You can also generate a whole array:

A length is also available in this case: LOOP_RESULT_LEN.

```c
int loop_10_times(int x) {
     int y[10] = {0};
     for (int i = 0; i < 10; i++) y[x++] = x;
     return y;
}

int *myvar = comp_array_int(LOOP_RESULT, loop_10_times(20));
```

Lastly, which a little bit of extra effort, you can also do it with structs.

```c
typedef struct {
     char *name;
     int year;
} Car;

// We need to generate the string
const char *print_car(Car c) {
        char *car = malloc(256);
        snprintf(car, 256, "(Car){ .name = %s, .int = %d }", c.name, c.year);
        return car;
}

// Returns the struct. The string is expected to be allocated, and is automatically freed.
// With this approach, you can reuse this struct to do something else.
Car mycar = comp_struct(MY_CAR, Car, print_car, .name = "Sedan Crown", .year = 1955);
```

3. Generate functions, or any code you want:

This will generate a function definition for strlen. Note the `//@` which allows you to insert the template inline, instead of having a separate file. This can be modified by changing the `MORPH_COMMENT` define.

This will generate to a file named "comptime.h", which can be modified.

```c
MORPH_GENERATE();

//@ int strlen(const char *s);
```

You can also loop this one-hundread times.

```c
for (int i = 0; i < 100; i++) {
     MORPH_GENERATE();
}

//@ int strlen(const char *s);
```

The user can also specify variables to replace the generated code:

```c
char *func_name = "strlen";
MORPH_GENERATE(BIND_STRING(func_name));

//@ int <func_name>(const char *s);
```

Reusing the same template is simple:

TODO: Conditionals. For instance, `strnlen` takes a second param, but strlen doesn't

```c
char *func_name = "strlen";
MORPH_GENERATE(BIND_STRING(func_name));
MORPH_GENERATE(SET_STRING(func_name, "strnlen"));

//@ int <func_name>(const char *s);
```

If you ever generate code that may allocate, don't excitate to use `defer`:

Defer will add its statements before calling return, or reaching the end of a scope.

```c
//@ int allocate_a_million_bytes(char *buff) {
//@      buff = malloc(1_000_000);
//@      if (buff == NULL) return -1;
//@      defer {
//@              free(buff);
//@      }
//@      char *message = "Hello, world!";
//@      memcpy(buff, message, strlen(message));
//@      printf("%s\n", buff);
//@      return 0;
//@ }
```

4. Generate macros:

You don't have to include the ending '\'

```c
char *macro_name = "add";
MORPH_DEFINE(BIND_STRING(macro_name));

//@ #define <macro_name>(a, b)
//@     a + b
```

5. TODO: Generate from stdout.

## TUnit

Zig's tests inspired testing unit. This library supports defining tests in header files and run all tests in a single process. TUnit also supports running each tests in its separate subprocess.

Currently, there is no support for running tests in several threads.

```c
// add.h
#include "tunit.h"
int add(int x, int y) { return x + y; }

// This macro wrap is important. You can either add test right below the function declaration, or
// at the ending
#ifdef TUNIT
TEST(add) {
  TASSERT(add(1,1) == 2, "expected two");
}
#endif

// add.c
#include "add.h"

// gcc -o add_test add.c -DTUNIT
```

Output:

```console
./add_test
 ✓ add 0.01ms
 === Test Summary ===
1 succeed, 0 failed and 0 skipped (0.02ms total)
```
