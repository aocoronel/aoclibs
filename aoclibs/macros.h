#ifndef AOCLIBS_MACROS_H
#define AOCLIBS_MACROS_H

#define string_literal_len(s) ((sizeof((s)) / sizeof((s)[0])) - sizeof((s)[0]))
#define array_len(a) sizeof(a) / sizeof(a[0])

#endif
