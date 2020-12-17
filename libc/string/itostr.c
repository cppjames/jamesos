#include <string.h>
#include <stdint.h>
#include <math.h>

size_t itostr(uint8_t str[], int32_t x, size_t base) {
	size_t nd = ndigits(x, base);
	size_t i = 0;
	
	if (x < 0)
		str[i++] = '-';
	x = abs(x);

	size_t end = i+nd-1;
	for (size_t j = 0; j < nd; j++) {
		uint8_t digit = x%base;
		uint8_t ch = (digit < 10) ? ('0' + x%base) : ('A' + x%base - 10);
		str[end-j] = ch;
		x /= base;
	}

	return end + 1;
}