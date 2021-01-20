#include <stdio.h>
#include <stdarg.h>

int f_printf(f_printch_t f_printch, const char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = f_vprintf(f_printch, format, parameters);

    va_end(parameters);
    return written;
}
