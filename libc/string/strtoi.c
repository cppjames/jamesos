#include <string.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

int strtoi(const char str[], size_t base) {
    bool negative = (*str == '-');
    if (negative) str++;

    unsigned int n = strtou(str, base);
    return negative ? (-n) : n;
}