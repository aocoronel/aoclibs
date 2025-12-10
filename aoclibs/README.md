# Type Aliases

The C standard library defines several integer types, providing a better and cleaner way to work with different integer types, such as `int32_t` and `int64_t`. Based on the style of new programming languages, I've aliased these values to a shorter counterpart in `int.h`, like `i32` and `i64`. It also includes aliases to `float`, `double` and `size_t`.

# Error Handling

## Introduction

From the various ways to handle errors in C, I came up with a fairly simple and enjoyable way to do so.

I know there may be better ways to handle errors in C, however it's fairly hard to create an error interface that allows me or any user that wants to use my method in a way it's dead simple and futile to handle errors.

Having to remember integer values for different error types, without an enum or defines, and then having to write a documentation on what error means. Each function is specific and may have very distinct errors, which makes this workflow cumbersome.

Setting a global variable is a brilliant idea, but only for single threaded applications, unless a thread-local feature is used. I don't really got into errno, but it also has another flaw: How are you supposed to know, when a function writes to errno? If it's not obviously written in the documentation, it's hard to tell, and will pass-through.

## Err Type

```c
typedef struct {
        i32 code;
        const char *msg;
} Err;
```

No brilliancy, just errno-like, but in a struct.

The biggest advantage of this approach is that the Err type gets explicitly set in the function definition, which makes it obvious, that the function returns an error.

No only to mention that it's trivial to set an Err or just write a predefined one with success using `err` and `ok`.

As an example:

```c
return err(-1, "File not found");
return ok(): // returns code 0, and message "OK"
```

> Yes, it's inspired by the Result<T,E> from Rust, specially Ok(()).

And, of course, you could wrap an enum or defines to get over with these magic error codes. This is up to you, because who can ever remember all errno codes?

Now, solving another problem, where a function has to return a value, but it can fail. There are a plenty of `[T]err` types defined in `error.h`, which include all classic types and my own aliased types. Whenever a library requires a different type, such a struct, it's straight-forward to add a new type. Here is an example:

```c
typedef struct { Err err; char value; }  echar;
typedef struct { Err err; char* value; } estr;
typedef struct { Err err; double value; } edouble;
typedef struct { Err err; float value; } efloat;
typedef struct { Err err; int value; } eint;
typedef struct { Err err; long value; }  elong;
typedef struct { Err err; long long value; }  elonglong;
typedef struct { Err err; short value; } eshort;
typedef struct { Err err; void* value; } evoid;
```

Although, it's easy to add a new type, but it's boring to add a macro or function to write each type, so I had to came with a easier way for `[T]err` types only:

```c
return ([T]){.err = err, .value = value};
```

I personally recommend to build the structure like above, rather than using a macro, for clarity.

## Helper Functions and Macros

I've already covered `err` and `ok`, but there are still a little more to cover.

`rerr` and `rerrln` are a clean way to write just the error message to the output you want, but for more sophisticated writes, use the `err.msg` directly.

Another useful error utility is `panic`. Panic will write the line, file and function that called panic and will abort the program. With the same implementation comes the `unreachable` macro, which calls `panic` with "unreachable" message.

Along with error handling utils, there is two new assert macros: `assert_err` and `assert_ok`, which conform with the default `assert` workings and ensure an error variable is a error or not. By "default workings", I mean `-DNDEBUG` will disable all asserts.
