# Convention

## Documentation

The documentation of all libraries are included within the header file.

I have not yet consolidated a good way to write good documentation in the header files. I make the docs as much natural as possible, explaining what the function does, if the name isn't explicit enough, what are the intrinsic you would only know by reading the code, sometimes tiny examples, return values and if there may be errno to check.

In the case of examples, I'm starting to use TUnit, which will be available at the end of each header files.

If there is any macro associated with a function I also like to keep it about its comment.

See example:

```c
// dir_walk("test.md", .islnk = my_fn);
#define dir_walk(path, ...) dir_walker(path, &(DirWalker){ __VA_ARGS__ })
// dir_walk can run four user provided functions based on each filetype:
// reg, directory, lnk and null.
//
// If the user prefers to ignore a certain filetype, the function values can be
// passed as NULL.
//
// dir_walker will not recurse by itself. You have to call dir_walker again inside
// the isdir function
//
// Sets errno << opendir
AOCDEF bool dir_walker(const char *restrict path, DirWalker *restrict dw);
```

In this case, `bool` is obvious enough, and I bother telling false is failure.

I don't have yet a convention to say when a function sets errno. If the function itself doesn't set, but it may be set by libc, I tell it anyway.

For telling a return value it's separated by `::`.

```c
// Returns the path corresponding to $HOME environment variable
// This function stores the getenv() output into a `static const char *` variable
//
// NULL :: failed to get env
AOCDEF const char *null get_home_env();
```

A new contender approached: `null`. In `aoclibs` all pointers that doesn't have `null` are assumes to be non nullable. It doesn't have any effect in the compiler, and no optimizations are applied. It's just a visual clue, rather a function can take a parameter as NULL or if it may return NULL. I try very hard to assert it everywhere, but I may miss a place here and there. In case of doubt, go read the source code.

## Header Files

Header files are defined in `base` and `libs`.

Libraries in `libs` may use other libraries from `base` and `libs`, but `base` libraries must not use from `libs`. You can consult the `template.h` to see what `base` libraries are standalones. Mostly will only depend on `base.h`, but since you can consume it as Unlicense, you can just copy paste it over.

The header guards are defined as follows: `AOC_LIBNAME_H_`.

## Functions

`base` libraries are not prefixed, but `libs` are. The reason is that `aoclibs` is not meant to be a dependency for already consolidated projects, but as some sort of a second standard library alternative to start a new project. In the early days, I found it to be very frustrating to add the `aoc` prefix all over the place, but since I'm the main consumer of my own library, I decided to simply remove all prefixes.

The documentation for each function is available in the header file. The documentation must describe what each function does, unless it's obvious enough, what defines can be set to apply special behavior, and describe the expected returned values or errors, if applicable.

```c
// Returns the index where 'pattern' starts in 's'.
// Assume strings are null-terminated.
//
// SIZE_MAX :: Pattern not found
//
// #NDEBUG disable assertions
AOCDEF size_t cstrstr_index(const char *restrict s, const char *restrict pattern);
```

## Macros

Macros may be `SCREAMING_SNAKE_CASE` when a macro works with compile-time expressions (e.g. `$strlen`) or a define can modify it's behavior (e.g. `$assert`). When a macro is used like a new "keyword" or a function, it's `snake_case`.

## Naming conventions

<!-- TODO: update function-like macro, since they are now $macro() -->

| Element                                            | Convention            |
| -------------------------------------------------- | --------------------- |
| Enum                                               | Pascal_Snake_Case     |
| Enum values                                        | Pascal_Snake_Case     |
| Function-like macro                                | snake_case            |
| Function-like macro, when #define changes behavior | SCREAMING_SNAKE_CASE  |
| Functions                                          | snake_case            |
| Global constant                                    | SCREAMING_SNAKE_CASE  |
| Local constant                                     | snake_case            |
| Macros (#define)                                   | SCREAMING_SNAKE_CASE  |
| Struct                                             | Pascal_Snake_Case     |
| Union                                              | Pascal_Snake_Case     |
| Variables                                          | snake_case            |

## Code Formatting

Take advantage of `.clang-format` to keep code consistency across all libraries. Some notable formatting style include:

- Tab indentation
- No brace wrapping (Allman style)
- 100 character column limit
- Inline, when possible
- Consistent spacing
