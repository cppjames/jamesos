#ifndef IDT_INTERNAL_H
#define IDT_INTERNAL_H

#include <stdint.h>

#define KERNEL_CODE_SEGMENT_OFFSET 0x08
#define INTERRUPT_GATE 0x8E

#define OFFSET_MASK_LOWER  0x000000000000ffffUL
#define OFFSET_MASK_MIDDLE 0x00000000ffff0000UL
#define OFFSET_MASK_HIGHER 0xffffffff00000000UL

typedef struct {
	uint16_t offsetLowerBits;
	uint16_t selector;
	uint8_t zero_1;
	uint8_t typeAttr;
	uint16_t offsetMiddleBits;
    uint32_t offsetHigherBits;
    uint32_t zero_2;
} __attribute__((packed)) IdtEntry;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IdtPtr;

void irqIsr0(void);
void irqIsr1(void);
void irqIsr2(void);
void irqIsr3(void);
void irqIsr4(void);
void irqIsr5(void);
void irqIsr6(void);
void irqIsr7(void);
void irqIsr8(void);
void irqIsr9(void);
void irqIsr10(void);
void irqIsr11(void);
void irqIsr12(void);
void irqIsr13(void);
void irqIsr14(void);
void irqIsr15(void);

#endif // IDT_INTERNAL_H