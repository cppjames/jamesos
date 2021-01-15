#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <devices/terminal.h>
#include <stdint.h>
#include <stdarg.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int vprintf(const char* __restrict, va_list parameters);

int putchar(int);
int puts(const char*);
void setcolor(enum vga_color);

#ifdef __cplusplus
}
#endif

#endif // _STDIO_H