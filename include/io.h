#ifndef AOCLIBS_IO_H
#define AOCLIBS_IO_H

#include "base.h"
#include <stdbool.h>
#include <stdio.h>

bool bputc(char *ref dst, size_t *ref len, size_t size, char src);
bool bputs(char *ref dst, size_t *ref dst_len, size_t dst_size, char *ref src, size_t src_len);
bool bputn(char *ref dst, size_t *ref len, size_t size, size_t count, char c);
bool bputw(char *ref dst, size_t *ref len, size_t size, size_t count);
bool bputf(char *ref dst, size_t *ref len, size_t size, int decimals, double f);
bool bputui(FILE *ref fp, char *ref dst, size_t *ref len, size_t size, unsigned long long n);
bool bputsi(FILE *ref fp, char *ref dst, size_t *ref len, size_t size, signed long long n);
size_t fputn(FILE *ref fp, size_t count, char c);
size_t fputw(FILE *ref fp, size_t count);
void fputf(FILE *ref fp, double f, int decimals);
void fputui(FILE *ref fp, unsigned long long n);
void fputsi(FILE *ref fp, signed long long n);

#ifdef AOCLIBS_IMPLEMENTATION
#include "io.c"
#endif

#endif
