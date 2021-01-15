#include <string.h>
#include <stdint.h>
#include <math.h>

size_t itostr(char str[], int x, size_t base) {
	size_t i = 0;
	
	if (x < 0)
		str[i++] = '-';
	x = abs(x);

	i += ztostr(str + i, (size_t)x, base);

	return i;
}