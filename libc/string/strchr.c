#include <string.h>

char* strchr(const char* str, char ch) {
	if (ch == '\0')
		return (char*)str + strlen(str);

    for (; *str; str++)
        if (*str == ch)
            return (char*)str;
    return 0;
}