#ifndef AOC_IO_H
#define AOC_IO_H

#include "base.h"

AOCDEF int eprintf(const char *fmt, ...) $attr_printf(1, 2);
AOCDEF int eprintfln(const char *fmt, ...) $attr_printf(1, 2);
AOCDEF int printfln(const char *fmt, ...) $attr_printf(1, 2);
AOCDEF bool bputc(char *dst, size_t *len, size_t size, char src);
AOCDEF bool bputs(char *dst, size_t *dst_len, size_t dst_size, char *src, size_t src_len);
AOCDEF bool bputn(char *dst, size_t *len, size_t size, size_t count, char c);
AOCDEF bool bputw(char *dst, size_t *len, size_t size, size_t count);
AOCDEF bool bputf(char *dst, size_t *len, size_t size, int decimals, double f);
AOCDEF bool bputui(char *dst, size_t *len, size_t size, unsigned long long n);
AOCDEF bool bputsi(char *dst, size_t *len, size_t size, signed long long n);
AOCDEF size_t fputn(FILE *fp, size_t count, char c);
AOCDEF size_t fputw(FILE *fp, size_t count);
AOCDEF void fputf(FILE *fp, double f, int decimals);
AOCDEF void fputui(FILE *fp, unsigned long long n);
AOCDEF void fputsi(FILE *fp, signed long long n);

#ifdef AOC_IMPLEMENTATION
#include "io.c"
#endif

#endif
