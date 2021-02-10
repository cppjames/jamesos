#include <kernel/stivale2.h>

#include <stdio.h>
#include <kernel/kinfo.h>
#include <kernel/kdebug.h>
#include <utils/debug.h>

#include <kernel/system/idt.h>
#include <kernel/system/gdt.h>
#include <kernel/shell.h>

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
	init_terminal();
    init_gdt();
    init_idt();

    print_splash_info(info);

    init_shell();
	
    for (;;)
        asm volatile ("hlt");
}
