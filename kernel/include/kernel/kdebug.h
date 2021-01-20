#ifndef KDBEUG_H
#define KDBEUG_H 1

#include <sys/cdefs.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int printch_port(int ch);

#define klog_debug(...) \
    f_printf(printch_port, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // KDBEUG_H