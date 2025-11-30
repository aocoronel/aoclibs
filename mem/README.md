# Memory

Many functions from `string.h` are considereed unsafe. The reason is because they really, heavily, on the null terminator. In case the null terminator is not there, so many functions can read or write off the bounds of the string and cause undefined behavior.

This module aims to address these problems by utilizing of a convention, instead of just a function implementation.

This module is still in development, and currently has important functions like `strlen`, `strcpy`, `strtok`, `strcmp` and `strcat` in my own implementation.

Off of these functions, there is `a_mem_convert.c`, which it's not yet proven, but aims to handle string conversion to other types in a simple way, and by returning a default value in case an error fails.

## Defining strings

In the `heap` module there is an API to handle strings based in the heap, and strings created with the String type are supposed to be used only by functions fully designed to handle the String type.

As a convention, all other strings should be declare as a constant with a compile-time known size:

```c
const char my_string[128];
```

It may not be necessary to heap allocate to make a simple buffered and formatted print to `stdout`, so this module rewrites common `string.h` functions with more checks.

For that, all functions requires the size of the string. Each function has a macro, which simplifies the process of adding the size, but they require defining the string with a known size at compile-time, thus the convention. Since getting the `sizeof(*str)` returns the size of the pointer, this is the only way that I found out to deal with it. At least, all operations are guaranteed to occur in the bounds of the string buffer, and never reach out-of-bounds.

## Functions

All the functions defined in this module can be used directly, like `_strlen`, or using a macro, which is the most appropriate way: `a_strlen`. All macros expect the string to have a size known at compile-time, so don't use the macro, if you have a pointer and you know it's size.
