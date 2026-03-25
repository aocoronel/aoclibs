#ifndef AOCLIBS_MATCH_H_
#define AOCLIBS_MATCH_H_

#ifdef AOCLIBS_MATCH

#include <stdbool.h>

// Match is useful when you have several if/else statements, and switch cases are not applicable
//
// bool match_str(char *x, char *y) {
//    return strcmp(x, y) == 0;
// }
//
// bool match_int(int x, int y) {
//    return x == y;
// }
//
// char *msg = "hello, world!";
//
// match(char *, msg, match_str) {
//      // This will fallthrough
//      when("hello") printf("msg is \"hello\");
//      // This won't fallthrough, if true
//      when("world") printf("plotting world domination");
//      else when("hello, world") printf("msg is \"hello\");
//      else printf("msg is something else");
// }
//
// int i = 0;
// int j = 1;
//
// match(int, i, match_int) {
//      when(0) printf("i is 0");
//      when(100) {
//              printf("i is 100);
//              break;
//      } else {
//              // Match can be nested as much as you want
//              match(int, j, match_int) {
//                      when(1) i = 0;
//                      // Must not be 0
//                      else unless(0) i = 0;
//              }
//      }
// }
#define match(val, func)                                   \
    for (struct {                                          \
             char iterator;                                \
             bool (*fn)(__typeof__(val), __typeof__(val)); \
             __typeof__(val) value;                        \
         } _Match = { 0, func, val };                      \
         _Match.iterator == 0;                             \
         _Match.iterator++)
#define when(val) (_Match.fn(_Match.value, val) == true)
#define unless(val) (_Match.fn(_Match.value, val) == false)
#endif

#endif
