#ifndef _STDIO_H
#define _STDIO_H 1

#include <sys/cdefs.h>
#include <vga.h>
#include <stdint.h>

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
void setcolor(enum vga_color);

#ifdef __cplusplus
}
#endif

#endif // _STDIO_H