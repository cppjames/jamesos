#include <kernel/stivale2.h>

#include <devices/keyboard.h>
#include <kernel/kdebug.h>
#include <kernel/kinfo.h>
#include <kernel/sched.h>
#include <kernel/shell.h>
#include <kernel/system/gdt.h>
#include <kernel/system/idt.h>
#include <kernel/system/sse.h>
#include <kernel/system/vmm.h>

#include <stdio.h>

static uint8_t stack[4096] = { 0 };
void kmain(StivaleStruct *info);

StivaleHTagSmp smpRequest = {
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
    .tags        = (uint64_t)&smpRequest
};


void kmain(StivaleStruct *info) {
    kinfoParseStivaleStruct(info);
    
    initTerminal();

    initGdt();
    initIdt();
    initPaging();
    initSse();

    initMultitasking();
    initKeyboard();

    kinfoPrintSplash(info);

    initShell();
	
    while (true)
        asm volatile ("hlt");
}
