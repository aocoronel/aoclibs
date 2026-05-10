# aoclibs

This repository contemplates C libraries developed specially for personal CLI development. This project is in constant development, although I use this almost everywhere, breaking changes may occur at any moment. It may take a while for this to become stable.

## How to use

```shell
# Generate a single header with all the libraries
gcc -o merge merge.c && ./merge
```

## Explore

- [Convention](./CONVENTION.md)

### Base libraries

- `arena.h` :: Linked-list arena *
- `base.h` :: Handful aliases and macros
- `colors.h` :: Terminal colors
- `cstr.h` :: C String operations
- `da.h` :: Heap/stack-based dynamic arrays *
- `file.h` :: File operations
- `fork.h` :: Fork high-level wrappers
- `fquery.h` :: "Regex" simple approach based on functions
- `io.h` :: Input/output simple functions
- `map.h` :: Key-value pairs
- `pp.h` :: Pointer Pool
- `printfc.h` :: `printf` with colors
- `rc.h` :: String type powered by `da.h`
- `slices.h` :: String slices

\* Based on [Tsoding](https://github.com/tsoding) code.

### Libraries

- [crown.h](./docs/crown.md) :: Full-blown CLI suite
- [tunit.h](./docs/tunit.md) :: Test Unit
- `heap_trace.h` :: Simple drop-in replacement for malloc/calloc/realloc to catch leaks
- `ini.h` :: INI parser
- `spinner.h` :: Simple spinner without allocations

## License

This repository is licensed under the MIT or Unlicense licenses, at your will, otherwise explicitly stated.
