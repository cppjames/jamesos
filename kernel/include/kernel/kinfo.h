#ifndef KINFO_H
#define KINFO_H

#include <kernel/stivale2.h>

enum klog_status {
    KLOG_SUCCESS,
    KLOG_INFO,
    KLOG_WARN,
    KLOG_FAIL,
    KLOG_PANIC
};

void print_splash_info(struct stivale2_struct *);
void klog_info(char*, enum klog_status);

#endif // KINFO_H