#ifndef KCONTROL_H
#define KCONTROL_H

#include <sys/cdefs.h>
#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((noreturn)) void _kpanic(char *format, ...);
void khalt();

#define STRINGIFY(X) #X
#define STRGY(X) STRINGIFY(X)

// TODO: Provide generic macros for debug-printing.
#define kpanic(...)                                                     \
    do {                                                                \
        klog_debug("[ %s: Function %s(): Line %s: Kernel panic\n] M! ", \
                   __FILE__, __func__, STRGY(__LINE__));                \
        klog_debug(__VA_ARGS__);                                        \
        klog_info(KLOG_PANIC, __VA_ARGS__);                             \
        for (;;) khalt();                                               \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // KCONTROL_H