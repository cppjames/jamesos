#include <string.h>
#include <stdint.h>

int memcmp(const void* aptr, const void* bptr, size_t size) {
	const uint8_t* a = (const uint8_t*) aptr;
	const uint8_t* b = (const uint8_t*) bptr;
	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}
