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

void kdebugHexdump64(void *address, size_t count);
void kdebugHexdump32(void *address, size_t count);
void kdebugHexdump16(void *address, size_t count);
void kdebugPutchar(char ch, __attribute__((unused)) void* arg);

#define kdebugLog(...) \
    fctprintf(kdebugPutchar, (void*)0, __VA_ARGS__)

void kbreakWaitKey();

#define kbreak(...)                            \
    do {                                       \
        printf("Function %s(): Line %s: [B] ", \
               __func__, STRGY(__LINE__));     \
        printf(__VA_ARGS__);                   \
        putchar('\n');                         \
        kbreakWaitKey();                      \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // KDBEUG_H