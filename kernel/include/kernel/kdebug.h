#ifndef KDBEUG_H
#define KDBEUG_H 1

#include <sys/cdefs.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STRINGIFY(X) #X
#define STRGY(X) STRINGIFY(X)

void kdebug_hexdump64(void *address, size_t count);
void kdebug_hexdump32(void *address, size_t count);
void kdebug_hexdump16(void *address, size_t count);
void printch_port(char ch, __attribute__((unused)) void* arg);

#define klog_debug(...) \
    fctprintf(printch_port, (void*)0, __VA_ARGS__)

void _kbreak();

#define kbreak(...)                            \
    do {                                       \
        printf("Function %s(): Line %s: [B] ", \
               __func__, STRGY(__LINE__));     \
        printf(__VA_ARGS__);                   \
        putchar('\n');                         \
        _kbreak();                             \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // KDBEUG_H