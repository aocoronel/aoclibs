# aoclibs

This repository contemplates C libraries developed specially for personal CLI development.

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
- `map.h` :: Key-value pairs
- `io.h` :: Input/output simple functions
- `pp.h` :: Pointer Pool
- `printfc.h` :: `printf` with colors
- `rc.h` :: String type powered by `da.h`
- `slices.h` :: String slices

\* Based on [Tsoding](https://github.com/tsoding) code.

### Libraries

- [crown.h](./docs/crown.md) :: Full-blown CLI suite
- [match.h](./docs/match.md) :: Better `if`, worst `switch`
- [tunit.h](./docs/tunit.md) :: Test Unit
- `heap_trace.h` :: Simple drop-in replacement for malloc/calloc/realloc to catch leaks
- `ini.h` :: INI parser
- `spinner.h` :: Simple spinner without allocations

## License

This repository is licensed under the MIT or Unlicense licenses, at your will, otherwise explicitly stated.
