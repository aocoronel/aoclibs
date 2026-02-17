# aoclibs

This repository contemplates C libraries developed specially for personal CLI development.

## Explore

- [Convention](./CONVENTION.md)

### Base libraries

- `arena.h` :: Linked list arena *
- `base.h` :: Handful aliases and macros
- `colors.h` :: Terminal colors
- `cstr.h` :: Safer string functions
- `da.h` :: Heap/stack-based dynamic arrays *
- `file.h` :: File operations
- `printfc` :: `printf` with colors
- `rc.h` :: String type powered by `da.h`

\* Based on [Tsoding](https://github.com/tsoding) code.

### Libraries

- [crown.h](./docs/crown.md) :: Full-blown CLI suite
- [match.h](./docs/match.md) :: Better `if`, worst `switch`
- [morph.h](./docs/morph.md) :: Deterministic code generator
- [tunit.h](./docs/tunit.md) :: Test Unit

## License

This repository is licensed under the MIT or Unlicense licenses, at your will.
