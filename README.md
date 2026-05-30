# aoclibs

> This project is in constant development, although I use this almost everywhere, breaking changes may occur at any moment. It may take a while for this to become stable.

I'm the main consumer of this library. As I refuse to write in many modern programming languages, I stick to simplicity of C. However, relying solely in `libc` and also implement a tiny standard library in every single project I do in C was becoming very common, I just decided to develop a more robust and feature rich standard library for myself to enjoy programming in C more.

The goal is to make writing programs in C more enjoyable, and decrease the amount of external third-party dependencies in the project. As of now, `aoclibs` does only depends on `libc` and in a few places, it uses `linux` specific APIs.

## Who is this library for?

`aoclibs` is supposed to be used in new projects, mainly by vendoring the generated `aoclibs.h` as of the single header stb-style.

This library is only tested in x86_64 and aarch64 Linux. If you are a C++ developer, or you write for Windows or MacOS, this library will certainly not work. I have no plans to support these (feel free to fork this project, if you want it). However, I may add support for other operational systems, as for now I'm still experimenting with FreeBSD.

## How to use

```shell
# Generate a single header with all the libraries
gcc -Isrc -Ilibs -Iinclude -o merge merge.c && ./merge
```

## Explore

- [Convention](./CONVENTION.md)

### Base libraries

- `arena.h` :: Linked-list arena *
- `base.h` :: Handful aliases and macros
- `buddy.h` :: Buddy allocator
- `colors.h` :: Terminal colors
- `cstr.h` :: C String operations and slices
- `da.h` :: Heap/stack-based dynamic arrays *
- `file.h` :: File operations
- `fork.h` :: Fork high-level wrappers
- `fquery.h` :: "Regex" simple approach based on functions
- `io.h` :: Input/output simple functions
- `map.h` :: Key-value pairs
- `pp.h` :: Pointer Pool
- `printfc.h` :: `printf` with colors
- `rc.h` :: String type powered by `da.h`
- `stack.h` :: Stack allocator

\* Based on [Tsoding](https://github.com/tsoding) code.

### Libraries

- [crown.h](./docs/crown.md) :: Full-blown CLI suite
- [tunit.h](./docs/tunit.md) :: Test Unit
- `heap_trace.h` :: Simple drop-in replacement for malloc/calloc/realloc to catch leaks
- `ini.h` :: INI parser
- `spinner.h` :: Simple spinner without allocations

## AI Disclosure

For transparency, this project is assisted by AI chatbots. The implied definition of the word "assisted", includes consulting AI to review and find bugs exclusively in C code. The implied definition doesn't include generating code by AI to speedup development, nor commit without human revision.

## License

This repository is licensed under the MIT or Unlicense licenses, at your will, otherwise explicitly stated.
