#ifndef AOCLIBS_ASSERT_H
#define AOCLIBS_ASSERT_H

#include <a_error.h>

#ifdef NDEBUG
#define assert_ok(e) ((void)0)
#define assert_err(e) ((void)0)
#else
#define assert_ok(err) \
    ((err).code == 0 ? (void)0 : _assert_ok(err, __FILE__, __LINE__, __func__))
#define assert_err(err) \
    ((err).code == 0 ? (void)0 : _assert_err(err, __FILE__, __LINE__, __func__))
#endif

void _assert_ok(Error e, const char *file, int line, const char *func);
void _assert_err(Error e, const char *file, int line, const char *func);

#endif
