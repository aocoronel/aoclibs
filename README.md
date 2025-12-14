# aoclibs

This repository contemplates C libraries developed specially for personal CLI development.

## Conventions

### Documentation

A README is being written for each module, explaining each function. I do encourage you to read the actual code and header files, since they are already a source of documentation themselves.

Whenever the documentation is not explanatory enough, the corresponding test file of save given name at `tests/` shows a real usage for each function.

### Header Files

Header files are defined in `aoclibs/` and separated by module. Each module has header files, which combines related functions, types and macros.

The header guards are defined as follows: `AOCLIBS_MODULE_FILENAME_H`.

### Functions

Functions will _almost_ start with the module name, followed by a short, yet descriptive name. Some functions may not follow this convention, and will, however, receive the filename as it's name. Another case, where this convention doesn't apply, is when a function starts with underscore. This means the function has a **must-use** macro by appending the letter `a`, like `strlen`.

The documentation for each function is available in the header file. The documentation must describe what each function does, what defines can be set to apply special behavior, and describe the expected returned values or errors, if applicable.

### Macros

Whenever a function starts with `_`, it means there is a macro which adds an abstraction to this function, and it's meant to be used.

### Tests

Tests are written in the `test/` directory, and are structured in the same way as `aoclibs/`, and have test filenames in the same name as the header files.

To run tests, [tunit](https://codeberg.org/aocoronel/tunit) is required.

### Naming conventions

| Element                                            | Convention           | Example           |
| -------------------------------------------------- | -------------------- | ----------------- |
| Functions                                          | snake_case           | io_print_indent() |
| Variables                                          | snake_case           | count             |
| Constant                                           | SCREAMING_SNAKE_CASE | TERM_WIDTH        |
| Function-like macro                                | snake_case           | strlen()        |
| Function-like macro, when #define changes behavior | SCREAMING_SNAKE_CASE | ASSERT_OK()       |
| Macros (#define)                                   | SCREAMING_SNAKE_CASE | AOCLIBS_ERROR_H   |
| Global variables                                   | snake_case           | error_count       |
| Static variables                                   | snake_case           | error_count       |
| Typedef names                                      | PascalCase           | FileType          |
| Enum                                               | PascalCase           | HeapError         |
| Enum values                                        | PascalCase           | OutOfMemory       |
| Struct                                             | PascalCase           | HeapError         |
| Struct members                                     | snake_case           | msg               |
| Union                                              | PascalCase           | HeapError         |
| Union members                                      | snake_case           | count             |

Some exceptions may apply. For example:

- When defining an enum and a string table to cover errors, the enum may start with \_PascalCase, and the string table may use camelCase, like eStringErr.

## Code Formatting

Take advantage of `.clang-format` to keep code consistency across all libraries. Some notable formatting style include:

- 8 character indent
- No brace wrapping
- 80 character column limit
- Inline, when possible
- Consistent spacing

## License

This repository is licensed under the MIT License, allowing for extensive use, modification, copying, and distribution.
