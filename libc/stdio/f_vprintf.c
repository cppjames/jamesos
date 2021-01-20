#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

enum specifier {
    FS_CHR = 	'c',
    FS_STR = 	's',
    FS_DEC =	'd',
    FS_OCT =	'o',
    FS_HEX =	'x',
    FS_BIN =	'b',
    FS_PTR =	'p'
};

enum ftype {
    FT_CHR,
    FT_STR,
    FT_INT,
    FT_FLT,
    FT_PTR,
    FT_DEF
};

enum flag {
    F_LEFT =	1 << 0U,
    F_SIGN =	1 << 1U,
    F_SPACE =	1 << 2U,
    F_ZERO =	1 << 3U,
    F_PREF =	1 << 4U,
};

typedef struct _chr_spec {
    char ch;
} _chr_spec;

typedef struct _str_spec {
    const char* str;
} _str_spec;

typedef struct _int_spec {
    char str[sizeof(int)*8 + 1];
    int n;
    size_t base;
    bool upper;
} _int_spec;

typedef struct _ptr_spec {
    char str[sizeof(size_t)*8 + 1];
    const void *p;
} _ptr_spec;

typedef union _typespec {
    _chr_spec chr_spec;
    _str_spec str_spec;
    _int_spec int_spec;
    _ptr_spec ptr_spec;
} _typespec;

typedef struct _fspec {
    const char* fbegin;
    enum ftype type;
    _typespec tspec;
    unsigned short flags;
    unsigned int minw;
    size_t len;
} _fspec;

static _fspec* _fspec_new(_fspec* fspec) {
    fspec->fbegin = (char*)0;
    fspec->type = FT_DEF;
    fspec->tspec = (_typespec){ 0 };
    fspec->flags = 0U;
    fspec->minw = 0U;
    fspec->len = 0U;

    return fspec;
}

static char* _parse_format(const char *format, _fspec* fspec) {
    fspec->fbegin = format++;

    for (;;) {
        if (*format == '-') {
            fspec->flags |= F_LEFT;
            format++;
        } else if (*format == '0') {
            fspec->flags |= F_ZERO;
            format++;
        } else
            break;
    }

    fspec->minw = strtou(format, 10);
    if (fspec->minw) format += ndigits(fspec->minw, 10);

    return (char*)format;
}

static bool print(f_printch_t f_printch, const char *data, size_t length) {
    const unsigned char *bytes = (const unsigned char *)data;
    for (size_t i = 0; i < length; i++)
        if (f_printch(bytes[i]) == EOF)
            return false;
    return true;
}

static inline size_t compute_pad(_fspec* fspec) {
    return (fspec->len < fspec->minw) ?
           (fspec->minw - fspec->len) : 0;
}

static bool printpad(f_printch_t f_printch, _fspec* fspec) {
    size_t pad = compute_pad(fspec);
    char* pad_str = (fspec->flags & F_ZERO) ? "0" : " ";
    for (; pad; pad--)
        if (!print(f_printch, pad_str, sizeof(char)))
            return false;
    return true;
}

static void _set_fspec_chr(_fspec* fspec, const char ch) {
    fspec->type = FT_CHR;
    fspec->tspec.chr_spec.ch = ch;
    fspec->len = sizeof ch;
}

static void _set_fspec_str(_fspec* fspec, const char* str) {
    fspec->type = FT_STR;
    fspec->tspec.str_spec.str = str;
    fspec->len = strlen(str);
}

static void _set_fspec_int(_fspec* fspec, const int n, const size_t base) {
    fspec->type = FT_INT;
    fspec->tspec.int_spec.n = n;
    fspec->tspec.int_spec.base = base;
    fspec->len = ndigits(n, base) + (n < 0);
}

static void _set_fspec_ptr(_fspec* fspec, const void* const p) {
    fspec->type = FT_PTR;
    fspec->tspec.ptr_spec.p = p;
    fspec->len = zdigits((size_t)p, 16);
}

static void _set_fspec_def(_fspec* fspec) {
    fspec->type = FT_DEF;
    fspec->tspec.str_spec.str = fspec->fbegin;
    fspec->len = strlen(fspec->fbegin);
}

static inline bool _is_fspec_def(_fspec* fspec) {
    return fspec->type == FT_DEF;
}

static inline bool _is_left_flag(_fspec* fspec) {
    return fspec->flags & F_LEFT;
}

static void _set_fspec(_fspec* fspec, const char* restrict format, va_list parameters) {
    if (*format == FS_CHR)
        _set_fspec_chr(fspec, (char)va_arg(parameters, int));
    else if (*format == FS_STR)
        _set_fspec_str(fspec, va_arg(parameters, const char*));
    else if (*format == FS_DEC)
        _set_fspec_int(fspec, va_arg(parameters, int), 10);
    else if (*format == FS_OCT)
        _set_fspec_int(fspec, va_arg(parameters, int), 8);
    else if (*format == FS_HEX)
        _set_fspec_int(fspec, va_arg(parameters, int), 16);
    else if (*format == FS_BIN)
        _set_fspec_int(fspec, va_arg(parameters, int), 2);
    else if (*format == FS_PTR)
        _set_fspec_ptr(fspec, va_arg(parameters, void*));
    else
        _set_fspec_def(fspec);
}

inline static bool _print_fspec_chr(f_printch_t f_printch, _fspec *fspec) {
    const char ch = fspec->tspec.chr_spec.ch;
    return print(f_printch, &ch, sizeof(ch));
}

inline static bool _print_fspec_str(f_printch_t f_printch, _fspec *fspec) {
    const char* str = fspec->tspec.str_spec.str;
    return print(f_printch, str, fspec->len);
}

inline static bool _print_fspec_int(f_printch_t f_printch, _fspec *fspec) {
    int n = fspec->tspec.int_spec.n;
    size_t base = fspec->tspec.int_spec.base;
    char* str = fspec->tspec.int_spec.str;
    itostr(str, n, base);
    return print(f_printch, str, fspec->len);
}

inline static bool _print_fspec_ptr(f_printch_t f_printch, _fspec *fspec) {
    size_t z = (size_t)fspec->tspec.ptr_spec.p;
    char* str = fspec->tspec.ptr_spec.str;
    ztostr(str, z, 16);
    return print(f_printch, str, fspec->len);
}

static bool _print_fspec_type(f_printch_t f_printch, _fspec* fspec) {
    if (fspec->type == FT_CHR)
        return _print_fspec_chr(f_printch, fspec);
    if (fspec->type == FT_STR)
        return _print_fspec_str(f_printch, fspec);
    if (fspec->type == FT_INT)
        return _print_fspec_int(f_printch, fspec);
    if (fspec->type == FT_PTR)
        return _print_fspec_ptr(f_printch, fspec);

    // Should never be reached
    return false;
}

static bool _print_fspec(f_printch_t f_printch, _fspec* fspec) {

    bool is_left_pad = !_is_left_flag(fspec) && printpad(f_printch, fspec);
    bool is_print = _print_fspec_type(f_printch, fspec);
    bool is_right_pad = _is_left_flag(fspec) && printpad(f_printch, fspec);

    return (is_left_pad || is_right_pad) && is_print;
}

int f_vprintf(f_printch_t f_printch, const char* restrict format, va_list parameters) {
    int written = 0;
    _fspec* fspec = &(_fspec){ 0 };

    while (*format) {
        size_t maxrem = INT_MAX - written;
        _fspec_new(fspec);

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount)
                goto ERR_OVERFLOW;
            
            if (!print(f_printch, format, amount))
                goto ERR_IO;
            format += amount;
            written += amount;
            continue;
        }

        format = _parse_format(format, fspec);
        _set_fspec(fspec, format, parameters);

        if (fspec->len > maxrem)
            goto ERR_OVERFLOW;

        if (_is_fspec_def(fspec))
            goto WRITE_DEFAULT;
        
        if (!_print_fspec(f_printch, fspec))
            goto ERR_IO;

        size_t total = fspec->len + compute_pad(fspec);
        written += total;

        format++;
    }

    goto EXIT;

WRITE_DEFAULT: ;
    if (!_print_fspec_str(f_printch, fspec))
        goto ERR_IO;
    written += fspec->len;
    goto EXIT;

ERR_OVERFLOW:
    written = -1;
    goto EXIT;

ERR_IO:
    written = -1;
    
EXIT:
    return written;
}
