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
    do { setcolor(VgaColor_LightCyan); \
        printf("%s() ", __func__); \
        setcolor(VgaColor_LightGray); \
        printf("%s", __FILE__); \
        setcolor(VgaColor_DarkGray); \
        printf(":"); \
        setcolor(VgaColor_LightBrown); \
        printf("%s", _STRGY(__LINE__)); \
        setcolor(VgaColor_DarkGray); \
        printf(": "); \
    } while (0)

#define _TRACE_PRINTF_H(...) \
    do { setcolor(VgaColor_LightGray); \
        _TRACE_PRINT_H(); \
        setcolor(VgaColor_White); \
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