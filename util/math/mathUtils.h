#ifndef MATH_UTILS_H
#define MATH_UTILS_H

#include "../struct/sharedStructs.h"

#define DELTA 0.00005

#define DELTA_COLOR 0.05

#define SWAP(x) ( ((x) << 24) | \
         (((x) << 8) & 0x00ff0000) | \
         (((x) >> 8) & 0x0000ff00) | \
         ((x) >> 24) )

#define FIX(x) (*(unsigned *)&(x) = \
         SWAP(*(unsigned *)&(x)))

long double pow2(long double num);
long double expTo(long double num, int exp);
long double min(long double a, long double b);
long double productoPunto(vector a, vector b);
long double absld(long double value);

#endif
