#ifndef AOCLIBS_IO_H
#define AOCLIBS_IO_H

#include "base.h"
#include <stdbool.h>
#include <stdio.h>

#pragma clang assume_nonnull begin
fn bool bputc(char *dst, size_t *len, size_t size, char src);
fn bool bputs(char *dst, size_t *dst_len, size_t dst_size, char *src, size_t src_len);
fn bool bputn(char *dst, size_t *len, size_t size, size_t count, char c);
fn bool bputw(char *dst, size_t *len, size_t size, size_t count);
fn bool bputf(char *dst, size_t *len, size_t size, int decimals, double f);
fn bool bputui(FILE *fp, char *dst, size_t *len, size_t size, unsigned long long n);
fn bool bputsi(FILE *fp, char *dst, size_t *len, size_t size, signed long long n);
fn size_t fputn(FILE *fp, size_t count, char c);
fn size_t fputw(FILE *fp, size_t count);
fn void fputf(FILE *fp, double f, int decimals);
fn void fputui(FILE *fp, unsigned long long n);
fn void fputsi(FILE *fp, signed long long n);
#pragma clang assume_nonnull end

#ifdef AOCLIBS_IMPLEMENTATION
#include "io.c"
#endif

#endif
