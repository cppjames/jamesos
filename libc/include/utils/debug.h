#ifndef _DEBUG_H
#define _DEBUG_H 1

#ifndef NDEBUG

#include <sys/cdefs.h>

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define _STRINGIFY(X) #X

#define _STRGY(X) _STRINGIFY(X)

#define _TRACE_PRINT_H() \
    do { setcolor(VGA_COLOR_LIGHT_CYAN); \
        printf("%s() ", __func__); \
        setcolor(VGA_COLOR_LIGHT_GREY); \
        printf("%s", __FILE__); \
        setcolor(VGA_COLOR_DARK_GREY); \
        printf(":"); \
        setcolor(VGA_COLOR_LIGHT_BROWN); \
        printf("%s", _STRGY(__LINE__)); \
        setcolor(VGA_COLOR_DARK_GREY); \
        printf(": "); \
    } while (0)

#define _TRACE_PRINTF_H(...) \
    do { setcolor(VGA_COLOR_LIGHT_GREY); \
        _TRACE_PRINT_H(); \
        setcolor(VGA_COLOR_WHITE); \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } while (0)

#define TRACE_PRINTF(...) \
    _TRACE_PRINTF_H(__VA_ARGS__);

#define TRACE_INSTR(...) \
    do { TRACE_PRINTS(_STRGY(__VA_ARGS__)); \
        __VA_ARGS__; \
    } while (0)

#else // NTRACE

#define TRACE_PRINTF(...) \
    do { /* Nothing */ } while (0)

#define TRACE_INSTR(...) \
    do { /* Nothing */ } while (0)

#endif // NTRACE

#define TRACE_PRINTS(S) \
    TRACE_PRINTF("%s", S);

#define TRACE_PTR(P) \
    TRACE_PRINTF("0x%p", (void*)P)

#endif // _DEBUG_H