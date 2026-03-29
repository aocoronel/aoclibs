#pragma once

#include "base.h"
#include "io.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AOCLIBS_IO_PUT_BUFF 64

bool bputc(char *dst, size_t *len, size_t size, char src) {
    ASSERT_NONNULL(dst != NULL);
    ASSERT_NONNULL(len != NULL);

    if (*len >= size) return false;
    dst[*len++] = src;
    return true;
}

bool bputs(char *dst, size_t *dst_len, size_t dst_size, char *src, size_t src_len) {
    ASSERT_NONNULL(dst != NULL);
    ASSERT_NONNULL(dst_len != NULL);
    ASSERT_NONNULL(src != NULL);

    if (*dst_len + src_len >= dst_size) return false;

    memcpy(dst + *dst_len, src, src_len);
    *dst_len += src_len;
    return true;
}

bool bputn(char *dst, size_t *len, size_t size, size_t count, char c) {
    ASSERT_NONNULL(dst != NULL);
    ASSERT_NONNULL(len != NULL);

    char space[AOCLIBS_IO_PUT_BUFF];
    memset(space, c, count);
    return bputs(dst, len, size, space, count);
}

bool bputw(char *dst, size_t *len, size_t size, size_t count) {
    ASSERT_NONNULL(dst != NULL);
    ASSERT_NONNULL(len != NULL);

    return bputn(dst, len, size, ' ', count);
}

bool bputf(char *dst, size_t *len, size_t size, int decimals, double f) {
    ASSERT_NONNULL(dst != NULL);
    ASSERT_NONNULL(len != NULL);

    if (f == 0.0f) {
        bool ret = bputc(dst, len, size, '0');
        if (!ret) return ret;
        if (decimals > 0) {
            bputc(dst, len, size, '.');
            ret = bputn(dst, len, size, decimals, '0');
        }
        return ret;
    }

    int negative = (f < 0);
    if (negative) {
        decimals++;
        f = -f;
    }

    int ipart = (int)f;
    int len_int = 0;
    {
        int temp = ipart;
        while (temp > 0) {
            temp /= 10;
            len_int++;
        }
    }

    double fpart = f - ipart;
    int len_frac = decimals;

    int total_len = len_int + (decimals > 0 ? 1 + len_frac : 0);
    char str[AOCLIBS_IO_PUT_BUFF];
    char *ptr = str + total_len;

    int frac_int = (int)(fpart * powf(10, decimals) + 0.5f);

    for (int i = 0; i < len_frac; i++) {
        *--ptr = '0' + (frac_int % 10);
        frac_int /= 10;
    }

    if (decimals > 0) *--ptr = '.';

    int n = ipart;
    while (n > 0) {
        *--ptr = '0' + (n % 10);
        n /= 10;
    }

    if (negative) *--ptr = '-';

    return bputs(dst, len, size, ptr, total_len);
}

bool bputui(char *dst, size_t *len, size_t size, unsigned long long n) {
    ASSERT_NONNULL(dst != NULL);
    ASSERT_NONNULL(len != NULL);

    if (n == 0) {
        return bputc(dst, len, size, '0');
    }

    int _len = 0;
    {
        int temp = n;
        while (temp > 0) {
            temp /= 10;
            len++;
        }
    }

    char str[AOCLIBS_IO_PUT_BUFF];
    char *ptr = str + _len;
    *ptr = '\0';

    ptr--;
    while (n > 0) {
        *ptr-- = '0' + (n % 10);
        n /= 10;
    }

    return bputs(dst, len, size, str, _len);
}

bool bputsi(char *dst, size_t *len, size_t size, signed long long n) {
    ASSERT_NONNULL(dst != NULL);
    ASSERT_NONNULL(len != NULL);

    if (n == 0) {
        return bputc(dst, len, size, '0');
    }

    int negative = n < 0;
    if (negative) {
        n = -n;
    }

    int _len = 0;
    {
        int temp = n;
        while (temp > 0) {
            temp /= 10;
            len++;
        }
    }

    char str[AOCLIBS_IO_PUT_BUFF];
    char *ptr = str + _len;
    *ptr = '\0';

    ptr--;
    while (n > 0) {
        *ptr-- = '0' + (n % 10);
        n /= 10;
    }

    if (negative) bputc(dst, len, size, '-');

    return bputs(dst, len, size, str, _len);
}

size_t fputn(FILE *fp, size_t count, char c) {
    ASSERT_NONNULL(fp != NULL);
    char space[AOCLIBS_IO_PUT_BUFF];
    memset(space, c, count);
    return fwrite(space, sizeof(char), count, fp);
}

size_t fputw(FILE *fp, size_t count) {
    ASSERT_NONNULL(fp != NULL);
    return fputn(fp, count, ' ');
}

void fputf(FILE *fp, double f, int decimals) {
    ASSERT_NONNULL(fp != NULL);

    if (f == 0.0f) {
        fputc('0', fp);
        if (decimals > 0) {
            fputc('.', fp);
            fputn(fp, decimals, '0');
        }
        return;
    }

    int negative = (f < 0);
    if (negative) {
        decimals++;
        f = -f;
    }

    int ipart = (int)f;
    int len_int = 0;
    {
        int temp = ipart;
        while (temp > 0) {
            temp /= 10;
            len_int++;
        }
    }

    double fpart = f - ipart;
    int len_frac = decimals;

    int total_len = len_int + (decimals > 0 ? 1 + len_frac : 0);
    char str[AOCLIBS_IO_PUT_BUFF];
    char *ptr = str + total_len;

    int frac_int = (int)(fpart * powf(10, decimals) + 0.5f);

    for (int i = 0; i < len_frac; i++) {
        *--ptr = '0' + (frac_int % 10);
        frac_int /= 10;
    }

    if (decimals > 0) *--ptr = '.';

    int n = ipart;
    while (n > 0) {
        *--ptr = '0' + (n % 10);
        n /= 10;
    }

    if (negative) *--ptr = '-';

    fwrite(ptr, sizeof(char), total_len, fp);
}

void fputui(FILE *fp, unsigned long long n) {
    ASSERT_NONNULL(fp != NULL);

    if (n == 0) {
        fputc('0', fp);
        return;
    }

    int len = 0;
    {
        int temp = n;
        while (temp > 0) {
            temp /= 10;
            len++;
        }
    }

    char str[AOCLIBS_IO_PUT_BUFF];
    char *ptr = str + len;
    *ptr = '\0';

    ptr--;
    while (n > 0) {
        *ptr-- = '0' + (n % 10);
        n /= 10;
    }

    fwrite(str, sizeof(char), len, fp);
}

void fputsi(FILE *fp, signed long long n) {
    ASSERT_NONNULL(fp != NULL);

    if (n == 0) {
        fputc('0', fp);
        return;
    }

    int negative = n < 0;
    if (negative) {
        n = -n;
    }

    int len = 0;
    {
        int temp = n;
        while (temp > 0) {
            temp /= 10;
            len++;
        }
    }

    char str[AOCLIBS_IO_PUT_BUFF];
    char *ptr = str + len;
    *ptr = '\0';

    ptr--;
    while (n > 0) {
        *ptr-- = '0' + (n % 10);
        n /= 10;
    }

    if (negative) fputc('-', fp);

    fwrite(str, sizeof(char), len, fp);
}
