# Debug

## Memory Debugger

`a_mem_debug.h` contains a couple of macros that replace `malloc`, `realloc`, `calloc` and `free` and store all operations.

Currently, this has a `print_memory_summary`, which writes a summary of how many allocations and frees have been made across the program execution. It also shows which functions allocated in the heap, in which file and line, that have not been freed.

This functionality is still to be further developed, to allow more expressive memory tracing. Although, this is never to be a `valgrind` replacement, but a complement that makes obvious the source of leak.

The only limitation is that it just looks for memory allocated by `malloc` and the other ones that were compiled by your own code, and not for other shared libraries.

