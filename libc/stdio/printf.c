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
	F_ZERO =	1 << 2U,
	F_PREF =	1 << 3U,
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
	for (size_t i = 0; i < pad; i++)
		if (!print(" ", sizeof(char)))
			return false;
	return true;
}

static inline size_t compute_pad(_fspec* fspec) {
	return (fspec->len < fspec->minw) ?
	       (fspec->minw - fspec->len) : 0;
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	size_t written = 0;
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

		if (*format == FS_CHR) {
			fspec->type = FT_CHR;
			fspec->tspec.chr_spec.ch = (char)va_arg(parameters, int);
			fspec->len = sizeof fspec->tspec.chr_spec.ch;
			/*
			format++;
			char c = (char)va_arg(parameters, int);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
			*/
		} else if (*format == FS_STR) {
			fspec->type = FT_STR;
			fspec->tspec.str_spec.str = va_arg(parameters, const char*);
			fspec->len = strlen(fspec->tspec.str_spec.str);
			/*
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(str, len))
				return -1;
			written += len;
			*/
		} else if (*format == FS_DEC) {
			fspec->type = FT_INT;
			fspec->tspec.int_spec.n = va_arg(parameters, int32_t);
			fspec->tspec.int_spec.base = 10;
			fspec->len = ndigits(fspec->tspec.int_spec.n, 10) + (fspec->tspec.int_spec.n < 0);
			/*
			format++;
			int32_t n = va_arg(parameters, int32_t);
			uint8_t str[sizeof(int)*8 + 1];
			
			size_t n_ch = ndigits(n, 10) + (n < 0);
			size_t pad = (n_ch < fspec->minw) ? (fspec->minw - n_ch) : 0;
			size_t len = n_ch + pad;

			size_t i = 0;
			
			if (!(fspec->flags & F_LEFT) && pad) {
				memset(str, ' ', pad);
				i += pad;
			}
			
			itostr(str + i, n, 10);

			if ((fspec->flags & F_LEFT) && pad)
				memset(str + n_ch, ' ', pad);
			
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print((char*)str, len))
				return -1;
			written += len;
			*/
		} else {
			fspec->type = FT_DEF;
			fspec->len = strlen(fspec->fbegin);
			/*
			format = format_begun_at;
			size_t len = strlen(format);
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, len))
				return -1;
			written += len;
			format += len;
			*/
		}

		if (fspec->len > maxrem) {
			// TODO: Set errno to EOVERFLOW.
			return -1;
		}

		format++;
		size_t pad = compute_pad(fspec);
		size_t total = fspec->len + pad;

		if (fspec->type == FT_DEF) {
			if (!print(fspec->fbegin, fspec->len))
				return -1;
		}

		if (!(fspec->flags & F_LEFT) && pad)
			if (!printpad(pad))
				return -1;

		if (fspec->type == FT_CHR) {
			const char ch = fspec->tspec.chr_spec.ch;
			if (!print(&ch, sizeof(ch)))
				return -1;
		} else if (fspec->type == FT_STR) {
			const char* str = fspec->tspec.str_spec.str;
			if (!print(str, fspec->len))
				return -1;
		} else if (fspec->type == FT_INT) {
			int32_t n = fspec->tspec.int_spec.n;
			size_t base = fspec->tspec.int_spec.base;
			char* str = fspec->tspec.int_spec.str;
			itostr(str, n, base);
			if (!print(str, fspec->len))
				return -1;
		}

		if ((fspec->flags & F_LEFT) && pad)
			if (!printpad(pad))
				return -1;

		written += total;
	}

	va_end(parameters);
	return written;
}
