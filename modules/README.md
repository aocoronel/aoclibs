# Modules

## Crown

Crown is a full-blown suite for creating CLIs. It includes an arrgument parser, Bash and Zsh autocompletion generators and help message generator.

## Morph

Non-obstructive compile-time implementation for C. This library can be used without dramatic changes. You can use it with simple macros like `comp_int`, `comp_char`, `comp_cstr`, `comp_array` and more. If you need to support other types, for simple ones a single line of macro is enough. However, if you want to support structs and unions, you will have to implementation the `comp` macro yourself.

```c
#define comp_intptr(...) comp("%zu", intptr_t, __VA_ARGS__)
```

Morph is a simple way to achieve compile-time in C, without overcomplicating anything. This is a standalone, and doesn't depend in anything else from AOCLIBS.

It doesn't require an external executable to generate code. The only thing you need is a C compiler.

To use it, you can create a comptime expression:

```c
int loop_10_times(int x) { for (int i = 0; i < 10; i++) return x; }
int myvar = comp_int(loop_10_times(20)); // returns 30
```

These macros won't break your code, and will allow you to run code at runtime. After running this, a file called "comptime.h" will be generated with the return value. If you now try to compile the program again, but with the `MORPH_USECOMP` define, the for loop is never compiled, since its result is cached.

What if you want to store a value in a global variable? Instead of using `comp_int`, you use the shared versions: `scomp_int`.

```c
int myvar = comp_int(FOR_LOOP_RESULT, loop_10_times(20)); // returns 30
```

Now you can use the result several times across your code by using: `usecomp`.

```c
int x = usecomp(FOR_LOOP_RESULT, 0);
//              ^                ^
//              define           fallback value
```

Because, the C compiler will block you from compiling the code, if `FOR_LOOP_RESULT` is not yet generated, and you also haven't provided the expression to generate it (this is a limitation for global variables), you have to provide a fallback value.

## TUnit

Zig's tests inspired testing unit. This library supports defining tests in header files and run all tests in a single process. TUnit also supports running each tests in its separate subprocess.

Currently, there is no support for running tests in several threads.

```c
// add.h
int add(int x, int y) { return x + y }

TEST(add) {
  TASSERT(add(1,1) == 2, "expected two");
}

// add.c
#include "add.h"

// gcc -o add_test add.c -DTUNIT
```
