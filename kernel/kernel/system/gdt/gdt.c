#include <kernel/system/gdt.h>
#include <stdint.h>

#include <kernel/kinfo.h>

extern int reload_segments();

uint64_t gdt[3] = {
    [0] = 0x000F00000000FFFF, // Null
    [1] = 0x00A09A0000000000, // 64 bit code
    [2] = 0x0000920000000000  // 64 bit data
};

typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_ptr_t;

gdt_ptr_t gdt_ptr = (gdt_ptr_t) {
    .limit = sizeof(gdt) - 1,
    .base = (uint64_t)&gdt[0]
};

void init_gdt() {
    asm volatile ("lgdt %0" :: "m" (gdt_ptr) : "memory");
    reload_segments();

    klog_info("GDT loaded.", KLOG_SUCCESS);
}