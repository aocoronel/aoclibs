# TUnit

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
  $tassert(add(1,1) == 2, "expected two");
}
#endif

// add.c
#include "add.h"

// gcc -o add_test add.c -DTUNIT
```

Output:

Notice we just compile the entirety of `tunit.h` without optimization flags, and the execution was fast.

```console
./add_test
 ✓ add 0.01ms
 === Test Summary ===
1 succeed, 0 failed and 0 skipped (0.02ms total)
```
