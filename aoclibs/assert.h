#ifndef AOCLIBS_ASSERT_H
#define AOCLIBS_ASSERT_H

#ifdef NDEBUG
#define ASSERT(exp, fmt, ...) ((void)0)
#else
#define ASSERT(exp, ...)    \
        (void)((!!(exp)) || \
               (_assert(#exp, __FILE__, __LINE__, __func__, fmt, __VA_ARGS__), 0))
#endif

void _assert(const char *expr, const char *file, unsigned line,
             const char *func, const char *fmt, ...);

#endif
