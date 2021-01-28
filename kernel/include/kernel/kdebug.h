#ifndef KDBEUG_H
#define KDBEUG_H 1

#include <sys/cdefs.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

void printch_port(char ch, __attribute__((unused)) void* arg);

#define klog_debug(...) \
    fctprintf(printch_port, (void*)0, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif // KDBEUG_H