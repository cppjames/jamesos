#include <math.h>
#include <stdint.h>

size_t ndigits(int x, size_t base) {
	x = abs(x);
	size_t nd = 1;
	while (x /= base)
		nd++;
	return nd;
}