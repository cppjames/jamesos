#include <kernel/interrupts.h>
#include <sys/io.h>
#include <stdint.h>

#include <utils/debug.h>

#define KERNEL_CODE_SEGMENT_OFFSET 0x0
#define INTERRUPT_GATE 0x8E

#define OFFSET_MASK_LOWER  0x000000000000ffff
#define OFFSET_MASK_MIDDLE 0x00000000ffff0000
#define OFFSET_MASK_HIGHER 0xffffffff00000000

typedef struct {
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero_1;
	uint8_t type_attr;
	uint16_t offset_middlebits;
    uint32_t offset_higherbits;
    uint32_t zero_2;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_ptr_t;

idt_entry_t idt[256];

static inline void set_idt_entry(uint8_t index, uint64_t irq_address, uint16_t selector, uint8_t attributes) {
    idt[index] = (idt_entry_t) {
        .offset_lowerbits = irq_address & OFFSET_MASK_LOWER,
        .selector = selector,
        .zero_1 = 0,
        .type_attr = attributes,
        .offset_middlebits = (irq_address & OFFSET_MASK_MIDDLE) >> 16,
        .offset_higherbits = (irq_address & OFFSET_MASK_HIGHER) >> 32,
        .zero_2 = 0
    };
}

void initialize_idt() {
    extern int load_idt();

    extern int irq0();
    extern int irq1();
    extern int irq2();
    extern int irq3();
    extern int irq4();
    extern int irq5();
    extern int irq6();
    extern int irq7();
    extern int irq8();
    extern int irq9();
    extern int irq10();
    extern int irq11();
    extern int irq12();
    extern int irq13();
    extern int irq14();
    extern int irq15();
 
    /* remapping the PIC */
	outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 40);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    set_idt_entry(32, (uint64_t)irq0,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(33, (uint64_t)irq1,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(34, (uint64_t)irq2,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(35, (uint64_t)irq3,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(36, (uint64_t)irq4,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(37, (uint64_t)irq5,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(38, (uint64_t)irq6,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(39, (uint64_t)irq7,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(40, (uint64_t)irq8,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(41, (uint64_t)irq9,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(42, (uint64_t)irq10, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(43, (uint64_t)irq11, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(44, (uint64_t)irq12, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(45, (uint64_t)irq13, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(46, (uint64_t)irq14, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(47, (uint64_t)irq15, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);

	idt_ptr_t idt_ptr = (idt_ptr_t) {
    	.limit = (sizeof(idt_entry_t) * 256) - 1,
	    .base = (uint64_t)idt
    };

 	load_idt(idt_ptr);
}