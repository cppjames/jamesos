#include <string.h>

size_t strspn(const char* dest, const char* src) {
    size_t i = 0;
    for (; dest[i] && strchr(src, dest[i]); i++)
        ;

    return i;
}