#ifndef IDT_INTERNAL_H
#define IDT_INTERNAL_H

#include <stdint.h>

#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define INTERRUPT_GATE 0x8E

#define OFFSET_MASK_LOWER  0x000000000000ffffUL
#define OFFSET_MASK_MIDDLE 0x00000000ffff0000UL
#define OFFSET_MASK_HIGHER 0xffffffff00000000UL

typedef struct {
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero_1;
	uint8_t type_attr;
	uint16_t offset_middlebits;
    uint32_t offset_higherbits;
    uint32_t zero_2;
} __attribute__((packed)) IdtEntry;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IdtPtr;

void irqIsr0();
void irqIsr1();
void irqIsr2();
void irqIsr3();
void irqIsr4();
void irqIsr5();
void irqIsr6();
void irqIsr7();
void irqIsr8();
void irqIsr9();
void irqIsr10();
void irqIsr11();
void irqIsr12();
void irqIsr13();
void irqIsr14();
void irqIsr15();

#endif // IDT_INTERNAL_H