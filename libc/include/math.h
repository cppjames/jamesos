#ifndef _MATH_H
#define _MATH_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int abs(int n) {
	return (n < 0) ? -n : n;
}

static inline int min(int n, int m) {
	return (n < m) ? n : m;
}

static inline int max(int n, int m) {
	return (n > m) ? n : m;
}

static inline double fabs(double x) {
	return (x < 0) ? -x : x;
}

static inline double fmin(double x, double y) {
	return (x < y) ? x : y;
}

static inline double fmax(double x, double y) {
	return (x > y) ? x : y;
}

size_t ndigits(int, size_t);
size_t zdigits(size_t, size_t);

#ifdef __cplusplus
}
#endif

#endif // _MATH_H