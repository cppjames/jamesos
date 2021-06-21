#include <kernel/stivale2.h>

#include <stdio.h>
#include <kernel/kinfo.h>
#include <kernel/kdebug.h>

#include <devices/keyboard.h>
#include <kernel/system/idt.h>
#include <kernel/system/gdt.h>
#include <kernel/system/vmm.h>
#include <kernel/shell.h>

static uint8_t stack[4096] = { 0 };
void kmain(StivaleStruct *info);

StivaleHTagSmp smp_request = {
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_SMP_ID,
        .next       = 0
    },
    .flags = 0
};

__attribute__((section(".stivale2hdr"), used))
StivaleHeader header2 = {
    .entry_point = (uint64_t)kmain,
    .stack       = (uintptr_t)stack + sizeof(stack),
    .flags       = 0,
    .tags        = (uint64_t)&smp_request
};

void kmain(StivaleStruct *info) {
    kinfoParseStivaleStruct(info);
    
    initTerminal();
    initGdt();
    initIdt();
    initKeyboard();
    initPaging();

    kinfoPrintSplash(info);

    initShell();
	
    while (true)
        asm volatile ("hlt");
}
