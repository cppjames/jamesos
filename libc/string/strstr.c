#include <string.h>

char* strstr(const char* str, const char* substr) {
	size_t cnt = 0;
	const char* index = 0;

	for (; *str; str++) {
		if (*str == substr[cnt]) {
			if (!cnt)
				index = str;
			cnt++;
			if (!substr[cnt])
				return (char*)index;
		} else {
			cnt = 0;
			index = 0;
		}
	}

	return 0;
}