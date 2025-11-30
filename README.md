# Coronel's C Libraries

This repository contemplates C libraries developed specially for personal CLI development.

## How to use

A README is being written for each module, explaining each function. I do encourage you to read the actual code and header files, since they are already a source of documentation themselves.

Whenever the documentation is not explanatory enough, the corresponding `test.c` shows a real usage for each function.

## Style

Header files should be highly separated, and may allow the user to include just the parts of the library they find useful, thus minimizing unnecessary imports.

While macros help in managing this kind of style, I do prefer to use it as least as I can.

### Header Files

All header files start with `a_`, followed by their module name, and their type.

- `assert` defines assert macros.
- `fn` defines functions. The function name comes after.
- `macros` defines macros.
- `main` glue all headers from the module.
- `type` defines types.

### Functions

Functions starting with underscore means they have a macro, which starts with `a_`.

In the header files, all functions should explain what they do concisely, explain the returned values and errors, and what values the user should expect.

Whenever a function has special behavior based on macros, it should be clarified.

### Macros

Not all macros start with `a_`, but they do when they work on top of a function with a similar name.

Macros defined as `a_module_macros.h` are mere extra utilities, which may be useful to the user, but poses only as extra syntax, rather than real utility. However, all macros declared in other files must be reviewed and classified as strictly necessary to the module usage.

## Code Formatting

Take advantage of `.clang-format` to keep code consistency across all libraries. Some notable formatting guides used include:

- 8 character indent
- No brace wrapping
- 80 character column limit
- Use inline, when possible
- Consistent spacing

## License

This repository is licensed under the MIT License, allowing for extensive use, modification, copying, and distribution.
