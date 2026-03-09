# Convention

## Documentation

The documentation of all libraries are included within the header file. The README may contain some highlights from them and examples.

## Header Files

Header files are defined in `base` and `libs`.

Libraries in `libs` may use other libraries from `base` and `libs`, but `base` libraries must not use from `libs`.

The header guards are defined as follows: `AOCLIBS_LIBNAME_H_`.

## Functions

Functions are always named with the `aoc` prefix, and can be stripped by defining `AOCLIBS_STRIP_PREFIX`. An exception apply, to libraries in `libs`, which uses their names as prefix.

The documentation for each function is available in the header file. The documentation must describe what each function does, what defines can be set to apply special behavior, and describe the expected returned values or errors, if applicable.

```c
// Returns the index where 'pattern' starts in 's'.
// Assume strings are null-terminated.
//
// SIZE_MAX :: Pattern not found
//
// #NDEBUG disable assertions
AOCDEF size_t aoc_cstrstr_index(const char *xref s, const char *xref pattern);
```

## Macros

Macros may be `SCREAMING_SNAKE_CASE` when a macro works with compile-time expressions (e.g. `STRLEN`) or a define can modify it's behavior (e.g. `ASSERT`). When a macro is used like a new "keyword" or a function, it's `snake_case`.

## Naming conventions

| Element                                            | Convention           |
| -------------------------------------------------- | -------------------- |
| Enum                                               | PascalCase           |
| Enum values                                        | PascalCase           |
| Function-like macro                                | snake_case           |
| Function-like macro, when #define changes behavior | SCREAMING_SNAKE_CASE |
| Functions                                          | snake_case           |
| Global constant                                    | SCREAMING_SNAKE_CASE |
| Local constant                                     | snake_case           |
| Macros (#define)                                   | SCREAMING_SNAKE_CASE |
| Struct                                             | PascalCase           |
| Typedef names                                      | PascalCase           |
| Union                                              | PascalCase           |
| Variables                                          | snake_case           |

## Code Formatting

Take advantage of `.clang-format` to keep code consistency across all libraries. Some notable formatting style include:

- 8 character indent
- No brace wrapping
- 100 character column limit
- Inline, when possible
- Consistent spacing
