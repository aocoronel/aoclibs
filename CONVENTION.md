# Convention

This file includes all the convention used in this project. Since this library is unstable, this file changes often and the source code may not fully follow it!

## Documentation

The documentation of all libraries are included within the header file, and will not be provided in another way. Also, not all variables, types, macros or function may be fully documented, since many of them are self explanatory, and writing the obvious is redundant. Whenever the documentation itself is not good enough, always read the source code or the tests.

The documentation is handwritten for humans, not for tools or agents. They contain a little description about what the function/macro/variable/type is all about, may also include some relevant notes, return values, what macro defines change behavior and which errno is set, including if they are propagated by functions from `libc`.

See a general example on how it's done:

```c
// Usage: dir_walk("test.md", .islnk = my_fn);
#define dir_walk(path, ...)                           \
	do {                                              \
		DirWalker _walk = (DirWalker){ __VA_ARGS__ }; \
		dir_walker(path, &_walk);                     \
	} while (0)
// dir_walker can run five user provided functions based on each FileType.
//
// If the user prefers to ignore a certain filetype, the function values can be
// passed as NULL.
//
// dir_walker will not recurse by itself. You have to call dir_walker again inside
// the isdir function
//
// errno << opendir
AOCDEF bool dir_walker(const char *restrict path, DirWalker *restrict dw);
```

In here we leave a macro very close it, signaling to you it's the preferred way to use this function (the functions may start with `_` if they are not supposed to be used directly), and how to use the macro itself. We give a little description on how to use it, some extra worthy information about recursion and tell the user that in case the return value is `false`, the only way it's false is because `opendir` failed, and it sets `errno`.

If we wanted to say this function sets `errno`, we would say: `errno >> EBUSY`, but since the usage of `errno` is discourage by the POSIX standard, I don't do it, but also for the fact that `errno` is not extensible.

Instead of the programmatic style of telling how to use a parameter like in the Doxygen style (`@param`), we just wrap them in quotes, and when we tell what are the possible return values we write separating the value from what it means with `::`. Of course, these examples are bad, since I try to make error handling less of a burden.

```c
// Reads file, splitting the read buffer by the delimiter.
// Returns how many bytes has been read.
//
// Allocates to "buff". The user owns the allocation.
// The buffer is null terminated
//
// SIZE_MAX :: failed to allocate
// SIZE_MAX :: EOF
AOCDEF size_t read_by_delim(char **restrict buff,
							size_t *restrict size,
							const char delim,
							FILE *restrict fd);
```

Finally, if the behavior of the function changes based on macros, we should say:

```c
// #TUNIT_SUBPROCESS creates subprocess, else run in a single process.
static inline void __tunit_run_single_test(__TUnitTest *test);
```

### Nullable

Often in the `libc` documentation we find two keywords: `_Nullable` and `_Nonnull`. As far as I know, `clang` is the only compiler that makes use of these two keywords, and if they are used once, the compiler will complain about it all over the place. Since I only care about telling the user that this function accepts `NULL` or it may return `NULL`, the convention is that all functions that don't specify `null` in the pointer is considered to be Nonnull. If you ever find a function that may accept `NULL` or return it, but it wasn't specified, consider it to be a bug.

Often people handle `NULL` as invalid, but they just return failure, instead of crashing the program with an assertion. I personally find it to be wrong, because passing `NULL` is wrong API usage, and you should let the user know that.

### Private

I usually prefix private variables/macros/functions with one `_` or two. However, I personally dislike the concept of private. This is currently used, but may change in the future to very long names.

I like to write reusable code. This means that if I write an allocator, I should have many pieces on how to make it, and let the user make their own, if they want. Private is against that idea.

In the current state of this project, all the types that are also not supposed to be used directly are suffixed with `_t` and are `snake_case`.

### Unit Tests

`aoclibs` presents a novel way to write tests by writing them along the source code, which is powered by [tunit](https://codeberg.org/aocoronel/aoclibs/src/branch/main/docs/tunit.md).

## Header Files

Header files are defined in `base` and `libs`.

Libraries in `libs` may use other libraries from `base` and `libs`, but `base` libraries must not use from `libs`. You can consult the `template.h` to see what `base` libraries are standalones. Mostly will only depend on `base.h`.

The header guards are defined as follows: `AOC_LIBNAME_H_`.

Source files defined in `src/` use the `#pragma once` guards, because they are included in the equivalent header files. This is a duck-tape fix for recursive inclusion, due to how C works.

## Functions

`base` libraries are not prefixed, but `libs` are. The reason is that `aoclibs` is not meant to be a dependency for already consolidated projects, but as some sort of a second standard library alternative to start a new project. In the early days, I found it to be very frustrating to add the `aoc` prefix all over the place, but since I'm the main consumer of my own library, I decided to simply remove all prefixes.

## Macros

Macros may be `SCREAMING_SNAKE_CASE` when a macro define values. When they are function-like macros they should be written in `snake_case` and prefixed with `$` (e.g. `$strlen()`).

## Naming conventions

| Element                                            | Convention            |
| -------------------------------------------------- | --------------------- |
| Enum and Enum Values                               | Pascal_Snake_Case     |
| Macros                                             | SCREAMING_SNAKE_CASE  |
| Function-like macro                                | $snake_case           |
| Functions                                          | snake_case            |
| Global variables                                   | SCREAMING_SNAKE_CASE  |
| Struct                                             | Pascal_Snake_Case     |
| Union                                              | Pascal_Snake_Case     |
| Variables, struct and union members                | snake_case            |

## Code Formatting

Take advantage of `.clang-format` to keep code consistency across all libraries. Some notable formatting style include:

- Tab indentation
- No brace wrapping (Allman style)
- `100` character column limit
- Inline, when possible
- Consistent spacing

I personally prefer the column limit to be `80`, however after a very long time I increased it to `100`. I tend to make very long functions, have a very readable style, and this kills all the space I have. It's a little tradeoff, but at least my code isn't as hard to read as libc's `fprintf` code.

Tab indentation is chosen to decrease source code file size, and also to be flexible on each user notion on how many columns an indentation should have. Despise that, I also prefer to avoid nesting the code too much.
