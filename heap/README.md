# Heap

## Introduction

Heap allocating in C is a double-edge sword, It's incredibly handy to have such a powerful tool, but managing it incorrectly makes it a nightmare.

By learning C, I couldn't ignore how easy it is to shoot yourself in the foot, while using heap allocations. It leaks!

## HeapPtr Type

C doesn't track how large are allocations, neither occupied space or it's capacity.

```c
typedef struct HeapPtr {
    void *ptr;
    usize mem; // current allocated size
} HeapPtr;
```

This type is a generic way to store allocations and follow along with their capacity. This type is managed by the following helper functions:

```c
Error heap_malloc(HeapPtr *h, usize size);
Error heap_calloc(HeapPtr *h, usize count, usize size);
Error heap_realloc(HeapPtr *h, usize new_size);
void heap_free(HeapPtr *h);
```

`heap_malloc` works on top of `malloc`, by updating `mem` and setting the pointer to `ptr`. `heap_calloc` works on top of `calloc`. `heap_realloc` works on top of `realloc` and `heap_free` works on top of `free`.

## String Type

Strings in C is a major source of vulnerabilities and undefined behavior. This simple fact took me quite long time to get it, and because I knew just a little bit about C, and I used to think C was unsafe in its totality. Once I found myself entering the Rust realm, but the C simplicity was the killing feature, that made me drip away from the "memory-safe" language.

Once one starts to write memory leaks all the time, they may see light in Rust.

The String Type is all about storing the string metadata across all operations.

```c
typedef struct {
        HeapPtr heap;
        usize length; // current length (not counting '\0')
} String;

typedef struct {
        Error err;
        String value;
} stringerr;
```

`stringerr` is a type used to return an Error and a String Type from a function. If you want to know more about the Error type, read the error module [README](../error/README.md).

Under the hood, String stores the pointer in the `HeapPtr.ptr` and its length in `length`.

The mechanism of the String functions are basically like C's standard library ones.

<!-- TODO: Continue talking about String functions -->
