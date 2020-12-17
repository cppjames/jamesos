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
};

struct _fspec {
	enum ftype type;
	uint32_t flags;
};

static struct _fspec* _fspec_new(struct _fspec* fspec) {
	fspec->type = FT_DEF;
	fspec->flags = 0U;

	return fspec;
}

static bool print(const char *data, size_t length) {
	const uint8_t *bytes = (const uint8_t *)data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

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

		const char* format_begun_at = format++;

		bool left = (*format == '-');
		if (left) format++;

		size_t fw = 0;
		while (isdigit(*format))
			fw = fw*10 + (*(format++) - '0');

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(&c, sizeof(c)))
				return -1;
			written++;
		} else if (*format == 's') {
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
		} else if (*format == 'd') {
			format++;
			int32_t n = va_arg(parameters, int32_t);
			uint8_t str[sizeof(int)*8 + 1];
			
			size_t n_ch = ndigits(n, 10) + (n < 0);
			size_t pad = (n_ch < fw) ? (fw - n_ch) : 0;
			size_t len = n_ch + pad;			

			size_t i = 0;
			
			if (!left && pad) {
				memset(str, ' ', pad);
				i += pad;
			}
			
			itostr(str + i, n, 10);
			
			if (left && pad)
				memset(str + n_ch, ' ', pad);
			
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print((char*)str, len))
				return -1;
			written += len;
		} else {
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
		}
	}

	va_end(parameters);
	return written;
}
