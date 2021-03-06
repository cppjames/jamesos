#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

unsigned int strtou(const char str[], size_t base) {
    unsigned int n = 0;

    while (*str) {
        size_t d = 0;
        if (isdigit(*str))
            d = *str - '0';
        else if (isalpha(*str) && base > 10)
            d = 10 + tolower(*str) - 'a';
        else
            break;
        
        if (n > UINT32_MAX / 10 ||
            d > UINT32_MAX - n*10) {
            return n;
        }

        n = n*base + d;
        str++;
    }
    
    return n;
}