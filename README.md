# C CLI Libraries

This repository contemplates C libraries developed specially for personal CLI development.

## Rules

### Philosophy

This repository follows some rules to keep everything in order, not only for readability, but to easily allow picking any library and start using straight away.

1. **All libraries should be independent.**

- All libraries presented in this repository should work without relying on other third-party libraries, neither from this repository.
- In practice just by getting the code and the header files, should suffice, unless the library has it's own subdirectory. See [`Directory Structure`](#directory-structure).

2. **All libraries should do one thing, and do it well.**

- The quality is a paramount, and by making sure all libraries solves a single problem, it ensures higher quality.

3. **Libraries can be used together, even if they don't require it.**

- As an enforcement to the first philosophy, all libraries are independent, but not in a bubble. Some libraries may take advantage of assets in common.

### Directory Structure

The code organization has different directories depending on the purpose of each library.

**Current directories:**

- `info`: Anything to deal with printing messages as a primary goal.
- `automation`: Automate boring tasks.

**Special directories:**

- `tests`: It's not a library about testing, but a testing playground for existing libraries.

### Code Formating

Take advantage of `.clang-format` to keep code consistency across all libraries. Some notable formatting guides used include:

- 8 character indent
- No brace wrapping
- 80 character column limit
- Use inline, when possible
- Consistent spacing

## License

This repository is licensed under the MIT License, allowing for extensive use, modification, copying, and distribution.
