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

Non-obstructive compile-time implementation for C. This library can be used without dramatic changes. You can use it with simple macros like `comp_int`, `comp_char`, `comp_cstr`, `comp_array` and more. If you need to support other types, for simple ones a single line of macro is enough. However, if you want to support structs and unions, you will have to implementation the `comp` macro yourself.

> [!NOTE]
> These macros will define macros at run-time. If your program has these macros in parts of the code that are not always ran, it won't produce the necessary defines. It's recommended to have a special source file that will generate the necessary defines.

```c
#define comp_intptr(...) comp("%zu", intptr_t, __VA_ARGS__)
```

Morph is a simple way to achieve compile-time in C, without overcomplicating anything. This is a standalone, and doesn't depend in anything else from AOCLIBS.

It doesn't require an external executable to generate code. The only thing you need is a C compiler.

To use it, you can create a comptime expression:

```c
int loop_10_times(int x) {
     for (int i = 0; i < 10; i++) x++;
     return x;
}
int myvar = comp_int(loop_10_times(20)); // returns 30
```

These macros won't break your code, and will allow you to run code at runtime. After running this, a file called "comptime.h" will be generated with the return value. If you now try to compile the program again, but with the `MORPH_USECOMP` define, the for loop is never compiled, since its result is cached.

What if you want to store a value in a global variable? Instead of using `comp_int`, you use the shared versions: `scomp_int`.

```c
int myvar = scomp_int(FOR_LOOP_RESULT, loop_10_times(20)); // returns 30
```

Now you can use the result several times across your code by using: `usecomp`.

```c
int x = usecomp(FOR_LOOP_RESULT, 0);
//              ^                ^
//              define           fallback value
```

Because, the C compiler will block you from compiling the code, if `FOR_LOOP_RESULT` is not yet generated, and you also haven't provided the expression to generate it (this is a limitation for global variables), you have to provide a fallback value.

`morph` is also capable of generating anything through a different implementation:

```c
// #define MORPH_USECOMP // for comp_int
#include "morph.h"

// #include "comptime.h" // result of this program

int main(void) {
        int time = 20;
        bool boolean = false;
        char *host = "server-1";
        double cpu = comp_int(CPU1_USAGE, 325.0 / 5.0);
        MORPH_GENERATE(BIND_DOUBLE(cpu), BIND_STRING(host), BIND_INT(time), BIND_BOOL(boolean));

        boolean = true;
        cpu = comp_int(CPU2_USAGE, 699.99 / 5.0);
        host = "web-1";
        MORPH_GENERATE(BIND_DOUBLE(cpu), BIND_STRING(host), BIND_INT(time), BIND_BOOL(boolean));

        //@ // Timestamp: <time>PM
        //@ // Premium: <boolean>
        //@ // CPU Usage: <cpu:%.3lf>%
        //@ // Host: <host>

        char *allocator = "alloca";
        char *func_name = "_alloca";
        char *rettype = "void";
        MORPH_GENERATE(BIND_STRING(rettype), BIND_STRING(func_name));

        allocator = "malloc";
        func_name = "_malloc";
        MORPH_GENERATE(SET_STRING(rettype, "int"), BIND_STRING(func_name));

        //@ #include `<alloca.h>`
        //@ #include `<stdlib.h>`
        //@ <rettype> <func_name>(int size) {
        //@             char *ptr = (char *)<allocator>(size);
        //@             ptr[size + 1] = '\0'; // Segfault!
        //@             return;
        //@ }

        return 0;
}
```

Output:

```c
#define CPU1_USAGE 65
// Timestamp: 2PM
// Premium: false
// CPU Usage: 65.000%
// Host: server-1

#define CPU2_USAGE 139
// Timestamp: 2PM
// Premium: true
// CPU Usage: 139.000%
// Host: web-1

#include <alloca.h>
#include <stdlib.h>
void _alloca(int size) {
            char *ptr = (char *)(size);
            ptr[size + 1] = '\0'; // Segfault!
            return;
}

#include <alloca.h>
#include <stdlib.h>
int _malloc(int size) {
            char *ptr = (char *)(size);
            ptr[size + 1] = '\0'; // Segfault!
            return;
}
```

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
