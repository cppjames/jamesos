#include <string.h>

char* strchr(const char* str, char ch) {
    while (*str)
        if (*str == ch)
            return str;
    return 0;
}