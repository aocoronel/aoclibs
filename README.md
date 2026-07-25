# aoclibs

> This project is in constant development, although I use this almost everywhere, breaking changes may occur at any moment. It may take a while for this to become stable.

I'm the main consumer of this library. As I refuse to write in many modern programming languages, I stick to simplicity of C. However, relying solely in `libc` and also as I was implementing a tiny standard library in every single project I do in C was becoming very common, I just decided to develop a more robust and feature rich standard library for myself to enjoy programming in C more.

The goal is to make writing programs in C more enjoyable, and decrease the amount of external third-party dependencies in the project. As of now, `aoclibs` does only depends on `libc`, the third-party libraries are vendored in `vendor`, and in a few places, it uses POSIX specific APIs.

## Who is this library for?

`aoclibs` is supposed to be used in new projects, mainly by vendoring the generated `aoclibs.h` as of the single header stb-style.

This library is only tested in x86_64 and aarch64 Linux and x86_64 FreeBSD. If you work on Windows this library will certainly not work, not even with MinGW. It might work on MacOS (I can't test it), and I may add support to other operational systems (NetBSD, OpenBSD, Haiku, DragonflyBSD) at some point.

`aoclibs` can currently be compiled with C++ without `extern "C"`.

## How to use

```shell
# Generate a single header with all the libraries
cc @build && ./merge
# Generate with C++
c++ @build++ && ./merge
```

## Explore

- [Convention](./CONVENTION.md)

### Base libraries

| Library                    | Description                       |
|----------------------------|-----------------------------------|
| alloc.h                    | Allocator interface               |
| base.h                     | Handful aliases and macros        |
| buddy.h                    | Buddy allocator                   |
| calculus.h                 | Math                              |
| colors.h                   | Terminal colors                   |
| cstr.h                     | C String operations and slices    |
| da.h                       | Heap/stack-based dynamic arrays   |
| file.h                     | File operations                   |
| fork.h                     | Fork high-level wrappers          |
| fquery.h                   | "Regex"-like based on functions   |
| hmap.h                     | Unordered hashmap                 |
| io.h                       | Input/output simple functions     |
| libc.h                     | Most included libc libraries      |
| map.h                      | Ordered map                       |
| pp.h                       | Pointer Pool                      |
| types.h                    | Forward declared types            |
| printfc.h                  | printf() with colors              |
| rc.h                       | String type powered by da.h       |
| stack.h                    | Stack allocator                   |
| [tunit.h](./docs/tunit.md) | Test Unit                         |

### Libraries

| Library                    | Description                        |
|----------------------------|------------------------------------|
| [crown.h](./docs/crown.md) | Full-blown CLI suite               |
| debug.h                    | Simple signal handler              |
| ini.h                      | INI parser                         |
| lexer.h                    | Simple lexer                       |
| spinner.h                  | Simple spinner without allocations |

### Vendored

| Library                                            | Description                   | License |
|----------------------------------------------------|-------------------------------|---------|
| [arena.h](https://github.com/tsoding/arena)        | Linked-list arena             | MIT     |
| [rapidhash](https://github.com/Nicoshev/rapidhash) | General purpose hash function | MIT     |

## AI Disclosure

For transparency, this project is assisted by AI chatbots. The implied definition of the word "assisted", includes consulting AI to review and find bugs exclusively in C code. The implied definition doesn't include generating code by AI to speedup development, nor commit without human revision.

## License

This repository is licensed under the MIT License, allowing for extensive use, modification, copying, and distribution.
