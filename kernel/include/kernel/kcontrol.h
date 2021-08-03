#ifndef KCONTROL_H
#define KCONTROL_H

#include <kernel/kdebug.h>
#include <kernel/kinfo.h>

#include <stdbool.h>
#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((noreturn)) void _kpanic(char *format, ...);
void khalt(void);

#define STRINGIFY(X) #X
#define STRGY(X) STRINGIFY(X)

// TODO: Provide generic macros for debug-printing.
#define kpanic(...)                                                     \
    do {                                                                \
        kdebugLog("[ %s: Function %s(): Line %s: Kernel panic\n] M! ",  \
                   __FILE__, __func__, STRGY(__LINE__));                \
        kdebugLog(__VA_ARGS__);                                         \
        kinfoLog(Log_Panic, __VA_ARGS__);                               \
        while (true) khalt();                                           \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // KCONTROL_H