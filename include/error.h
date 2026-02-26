#ifndef AOCLIBS_ERROR_H_
#define AOCLIBS_ERROR_H_

// Error handling in C is rather limited. This is probably the shortest error handling
// implementation you have ever seen, and rather powerful.

// The idea is the user sets a special parameter in the function called "err".
//
// ```c
// #include <limits.h>
// #include <stdio.h>
//
// #define add(x, y) add_err(x, y, &err)
// int add_err(int x, int y, int *err) {
//     if (y > 0 && x > INT_MAX - y) {
//         *err = 1; // Overflow
//         return x + y;
//     }
//     if (y < 0 && x < INT_MIN - y) {
//         *err = -1; // Underflow
//         return x + y;
//     }
//     *err = 0; // No error
//     return x + y;
// }
//
// int main(void) {
//      int err = 0;
//
//      // No macro
//      add_err(INT_MAX, INT_MAX, &err) catch {
//              if (err == 1) fprintf(stderr, "Add operation overflew\n");
//              else if (err == -1) fprintf(stderr, "Add operation underflew\n");
//      }
//
//      // With macro
//      add(INT_MAX, INT_MAX) catch {
//              switch (err) {
//                 case 1:
//                         fprintf(stderr, "Add operation overflew\n");
//                         break;
//                 case -1:
//                         fprintf(stderr, "Add operation underflew\n");
//                         break;
//                 }
//              }
//      }
// }
// ```

// Visual clue
#define fault enum

// Catching errors
#define catch \
        ;     \
        if (err != 0)

#endif
