#include <kernel/gdt.h>
#include <stdint.h>

#include <kernel/kdebug.h>
#include <utils/debug.h>

#define BASE_MASK_LOWER  0x0000ffff
#define BASE_MASK_MIDDLE 0x00ff0000
#define BASE_MASK_HIGHER 0xff000000

typedef struct {
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_middle;
	uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

gdt_entry_t gdt[3];
uint64_t gdt_test[3];
gdt_ptr_t gdt_ptr;

inline static void set_gdt_entry(unsigned int index, uint64_t base, uint64_t limit, uint8_t access, uint8_t gran) {
    gdt[index] = (gdt_entry_t) {
        .base_low =    (base & BASE_MASK_LOWER),
        .base_middle = (base & BASE_MASK_MIDDLE) >> 16,
        .base_high =   (base & BASE_MASK_HIGHER) >> 24,

        .limit_low = (limit & 0x0000ffff),
        .granularity = ((limit & 0x000f0000) >> 16) | (gran & 0xF0),
        .access = access
    };
}

void initialize_gdt() {
    extern int reload_segments();

    //set_gdt_entry(0, 0, 0, 0, 0);                // NULL descriptor
    //set_gdt_entry(1, 0, 0xffffffff, 0x9A, 0xC0); // Code Segment
    //set_gdt_entry(2, 0, 0xffffffff, 0x92, 0xC0); // Data Segment
    
    gdt_test[0] = 0x000F00000000FFFF; // Null
    gdt_test[1] = 0x00A09A0000000000; // 64 bit code
    gdt_test[2] = 0x0000920000000000; // 64 bit data

    gdt_ptr = (gdt_ptr_t) {
        .limit = sizeof(gdt_test) - 1,
        .base = (uint64_t)&gdt_test[0]
    };

    asm volatile ("lgdt %0" :: "m" (gdt_ptr) : "memory");

    reload_segments();
}