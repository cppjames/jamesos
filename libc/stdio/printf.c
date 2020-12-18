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
	int32_t n;
	size_t base;
	bool upper;
} _int_spec;

typedef union _typespec {
	_chr_spec chr_spec;
	_str_spec str_spec;
	_int_spec int_spec;
} _typespec;

typedef struct _fspec {
	const char* fbegin;
	enum ftype type;
	_typespec tspec;
	uint32_t flags;
	uint32_t minw;
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

static bool print(const char *data, size_t length) {
	const uint8_t *bytes = (const uint8_t *)data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

static bool printpad(size_t pad) {
	for (; pad; pad--)
		if (!print(" ", sizeof(char)))
			return false;
	return true;
}

static inline size_t compute_pad(_fspec* fspec) {
	return (fspec->len < fspec->minw) ?
	       (fspec->minw - fspec->len) : 0;
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

static void _set_fspec_int(_fspec* fspec, const int32_t n, const size_t base) {
	fspec->type = FT_INT;
	fspec->tspec.int_spec.n = n;
	fspec->tspec.int_spec.base = base;
	fspec->len = ndigits(n, base) + (n < 0);
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;
	_fspec* fspec = &(_fspec){ 0 };

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;
		_fspec_new(fspec);

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		fspec->fbegin = format++;

		if (*format == '-') {
			fspec->flags |= F_LEFT;
			format++;
		}

		fspec->minw = strtou(format, 10);
		if (fspec->minw) format += ndigits(fspec->minw, 10);

		if (*format == FS_CHR)
			_set_fspec_chr(fspec, (char)va_arg(parameters, int));
		else if (*format == FS_STR)
			_set_fspec_str(fspec, va_arg(parameters, const char*));
		else if (*format == FS_DEC)
			_set_fspec_int(fspec, va_arg(parameters, int32_t), 10);
		else if (*format == FS_OCT)
			_set_fspec_int(fspec, va_arg(parameters, int32_t), 8);
		else if (*format == FS_HEX)
			_set_fspec_int(fspec, va_arg(parameters, int32_t), 16);
		else if (*format == FS_BIN)
			_set_fspec_int(fspec, va_arg(parameters, int32_t), 2);
		else {
			size_t len = strlen(fspec->fbegin);
			if (!print(fspec->fbegin, len))
				goto ERR_IO;
			written += len;
			goto EXIT;
		}

		if (fspec->len > maxrem)
			goto ERR_OVERFLOW;

		format++;
		size_t pad = compute_pad(fspec);
		size_t total = fspec->len + pad;

		if (!(fspec->flags & F_LEFT) && pad)
			if (!printpad(pad))
				goto ERR_IO;

		if (fspec->type == FT_CHR) {
			const char ch = fspec->tspec.chr_spec.ch;
			if (!print(&ch, sizeof(ch)))
				goto ERR_IO;
		} else if (fspec->type == FT_STR) {
			const char* str = fspec->tspec.str_spec.str;
			if (!print(str, fspec->len))
				goto ERR_IO;
		} else if (fspec->type == FT_INT) {
			int32_t n = fspec->tspec.int_spec.n;
			size_t base = fspec->tspec.int_spec.base;
			char* str = fspec->tspec.int_spec.str;
			itostr(str, n, base);
			if (!print(str, fspec->len))
				goto ERR_IO;
		}

		if ((fspec->flags & F_LEFT) && pad)
			if (!printpad(pad))
				goto ERR_IO;

		written += total;
	}

	goto EXIT;

ERR_OVERFLOW:
	written = -1;
	goto EXIT;
ERR_IO:
	written = -1;
	goto EXIT;
EXIT:
	va_end(parameters);
	return written;
}
