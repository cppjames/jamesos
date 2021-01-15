#include <kernel/interrupts.h>
#include <sys/io.h>
#include <stdint.h>

#define KERNEL_CODE_SEGMENT_OFFSET 0x80
#define INTERRUPT_GATE 0x8E

typedef struct idt_entry {
	uint16_t offset_lowerbits;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_higherbits;
} idt_entry_t;

typedef struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed)) idt_ptr_t;

idt_entry_t idt[256];

static inline void set_idt_entry(uint8_t index, unsigned long irq_address, uint16_t selector, uint8_t attributes) {
    idt[index] = (idt_entry_t) {
        .offset_lowerbits = irq_address & 0xffff,
        .selector = selector,
        .zero = 0,
        .type_attr = attributes,
        .offset_higherbits = (irq_address & 0xffff0000) >> 16
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

    set_idt_entry(32, (unsigned long)irq0,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(33, (unsigned long)irq1,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(34, (unsigned long)irq2,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(35, (unsigned long)irq3,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(36, (unsigned long)irq4,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(37, (unsigned long)irq5,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(38, (unsigned long)irq6,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(39, (unsigned long)irq7,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(40, (unsigned long)irq8,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(41, (unsigned long)irq9,  KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(42, (unsigned long)irq10, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(43, (unsigned long)irq11, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(44, (unsigned long)irq12, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(45, (unsigned long)irq13, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(46, (unsigned long)irq14, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);
    set_idt_entry(47, (unsigned long)irq15, KERNEL_CODE_SEGMENT_OFFSET, INTERRUPT_GATE);

	idt_ptr_t idt_ptr = (idt_ptr_t) {
    	.limit = (sizeof(idt_entry_t) * 256) - 1,
	    .base = (unsigned long)idt
    };

 	load_idt(idt_ptr);
}