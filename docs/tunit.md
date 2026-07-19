# TUnit

TUnit is probably the most powerful and lightweight testing unit library you will ever see. Although the usage looks very similar to [Criterion](https://github.com/Snaipe/Criterion). In fact, TUnit is a single header library. TUnit is not just simple, but also completely independent from `aoclibs`, so you can just download `tunit.h` and play around with it.

|                               | Criterion                                                                                       | TUnit                                                                         |
|-------------------------------|-------------------------------------------------------------------------------------------------|-------------------------------------------------------------------------------|
| LOC                           | C: 7559; Header: 10815 (0586ad3 - bleeding)                                                     | 313 (4524079 - main)                                                          |
| Dependencies                  | openssl, libffi, libgit2, libnanomsg, libllhttp, libpcre2, libssh2                              | None                                                                          |
| Build                         | 500 lines of Meson to make, then you need to link                                               | `cc -DTUNIT`                                                                  |
| Speed on single test `0 == 0` | 0.40ms                                                                                          | 0.02ms single/multiprocess (-O0)                                              |
| Ease of use                   | They have a documentation                                                                       | The only example you need is right below, you just need to know C to use this |
| Unique Features               | xUnit framework structure, parameterized tests and theories Subprocess tests only, report hooks | Define tests alongside the source code, and optionally detect leaks in tests  |
| Platform support              | Linux, FreeBSD, macOS, and Windows                                                              | Linux and FreeBSD                                                             |

`tunit.h` has an optional dependency on `heap_trace.h`, which finds `malloc`, `calloc`, `realloc` and `free` based leaks, and tells where in the source code that happened.

TUnit is inspired by the Zig test unit feature, that lets you write tests alongside the actual source code, instead of in a separate `test/` directory with separate tests files, that needs to be compiled separatedly. TUnit lets you compile all tests at once and run everything in a single process, or in subprocesses (trading-off `heap_trace` in the process).

Currently, there is no support for running tests in several threads.

> In aoclibs, tunit.h is included in template.h, so we can use tunit.h without even bother including it everywhere

Follow the only example you need:

```c
// add.c
#include "add.h"
int add(int x, int y) {
  return x + y;
}
```

```c
// add.h
#include "tunit.h"

int add(int x, int y);

// This macro wrap is important. You can either add test right below the function declaration, or
// at the ending
#ifdef TUNIT
TEST(add) {
  int x = 1;
  int y = 2;
  // raw messages are useless, embrace the power of printf
  $tunit_assert(add(x, y) == 2, "x: %d, y: %d", x, y);
  // aoclibs has direct support for tunit. So all $assert macros are converted to tunit asserts
  // automatically
  // $assert(add(x, y) == 2, "x: %d, y: %d", x, y);
}
#endif
```

```console
gcc -o add_test add.c -DTUNIT
```

Output:

```console
❯ ./add_test
 add.h:12: Assertion failed in test add: add(x, y) == 2: x: 1, y: 2
 fail: add 0.01ms
0 succeed, 1 failed (0.01ms total)
```

Notice we just compile the entirety of `tunit.h` without optimization flags, and the execution was fast.
