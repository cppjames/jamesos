#ifndef _MATH_H
#define _MATH_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define abs(X) (((X) < 0) ? (-(X)) : (X))
#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

size_t ndigits(int32_t, size_t);

#ifdef __cplusplus
}
#endif

#endif // _MATH_H