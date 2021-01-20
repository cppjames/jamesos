#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

int vprintf(const char* restrict format, va_list parameters) {
    return f_vprintf(putchar, format, parameters);
}
