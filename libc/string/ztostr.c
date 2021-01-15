#include <string.h>
#include <stdint.h>
#include <math.h>

size_t ztostr(char str[], size_t z, size_t base) {
	size_t nd = zdigits(z, base);
	size_t i = 0;

	size_t end = i+nd-1;
	for (size_t j = 0; j < nd; j++) {
		unsigned char digit = z % base;
		unsigned char ch = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
		str[end-j] = ch;
		z /= base;
	}

	return end + 1;
}