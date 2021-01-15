#include <string.h>

char* strncpy (char* dest, const char* src, size_t n) {
	size_t len = strlen(src);
	size_t size = len + 1;
	if (size >= n)
		size = n;
	else
		memset(dest + size, '\0', n - size);
	return memcpy(dest, src, size);
}