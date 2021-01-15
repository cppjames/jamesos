#include <math.h>
#include <stdint.h>

size_t zdigits(size_t z, size_t base) {
	size_t nd = 1;
	while (z /= base)
		nd++;
	return nd;
}