#include <kernel/stivale2.h>

#include <stdio.h>
#include "kinfo.h"

#include <sys/io.h>
#include <kernel/interrupts.h>

#include <utils/debug.h>

// Remove
#include <string.h>

static uint8_t stack[4096] = { 0 };
void kernel_main(struct stivale2_struct *info);

struct stivale2_header_tag_smp smp_request = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next       = 0
    },
    .flags = 0
};

__attribute__((section(".stivale2hdr"), used))
struct stivale2_header header2 = {
    .entry_point = (uint64_t)kernel_main,
    .stack       = (uintptr_t)stack + sizeof(stack),
    .flags       = 0,
    .tags        = (uint64_t)&smp_request
};

void kernel_main(__attribute__((unused)) struct stivale2_struct *info) {
	terminal_initialize();
    klog_info("Initialized terminal.", KLOG_SUCCESS);

    initialize_idt();
    klog_info("Loaded IDT.", KLOG_SUCCESS);

    print_splash_info(info);
	
    asm volatile ("hlt");
}
