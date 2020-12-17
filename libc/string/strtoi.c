#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

int32_t strtoi(uint8_t str[], size_t base) {
    uint32_t n = 0;

    while (*str) {
        size_t d = 0;
        if (isdigit(*str))
            d = *str - '0';
        else if (isalpha(*str) && base > 10)
            d = 10 + tolower(*str) - 'a';
        else
            break;
        
        if (n > INT32_MAX / 10 ||
            d > INT32_MAX - n*10) {
            // TODO: Set errno to EOVERFLOW.
            return n;
        }

        n = n*base + d;
    }
    
    return n;
}