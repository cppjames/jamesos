#include <kernel/stivale2.h>

#include <stdio.h>
#include <kernel/tty.h>
#include <vga.h>

static uint8_t stack[4096] = {0};
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

void kernel_main(struct stivale2_struct *info) {
	terminal_initialize();

    printf("%s", "Decimal, fw 7, left\n");
	printf("56        |%-7d|\n", 56);
	printf("-142      |%-7d|\n", -142);
	printf("1892      |%-7d|\n", 1892);
	printf("100014632 |%-7d|\n", 100014632);

    printf("%s", "\nDecimal, fw 7, right\n");
	printf("56        |%7d|\n", 56);
	printf("-142      |%7d|\n", -142);
	printf("1892      |%7d|\n", 1892);
	printf("100014632 |%7d|\n", 100014632);

    printf("%s", "\nChar, fw 3, left\n");
	printf("k  |%-3c|\n", 'k');
	printf("=  |%-3c|\n", '=');
	printf("%%  |%-3c|\n", '%');
	printf("]  |%-3c|\n", ']');

    printf("%s", "\nString, fw 12, right\n");
	printf("hey bebi           |%12s|\n", "hey bebi");
	printf("is it working now? |%12s|\n", "is it working now?");
	printf("ok                 |%12s|\n", "ok");
	printf("                   |%12s|\n", "");

	asm volatile ("hlt");
}
