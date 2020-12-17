#ifndef __SYS__CPU_H__
#define __SYS__CPU_H__

#include <stdint.h>
#include <stdbool.h>

#define FLAT_PTR(PTR) (*((int(*)[])(PTR)))

#define BYTE_PTR(PTR)  (*((uint8_t *)(PTR)))
#define WORD_PTR(PTR)  (*((uint16_t *)(PTR)))
#define DWORD_PTR(PTR) (*((uint32_t *)(PTR)))
#define QWORD_PTR(PTR) (*((uint64_t *)(PTR)))

static inline bool cpuid(uint32_t leaf, uint32_t subleaf,
          uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx) {
    uint32_t cpuid_max;
    asm volatile ("cpuid"
                  : "=a" (cpuid_max)
                  : "a" (leaf & 0x80000000)
                  : "ebx", "ecx", "edx");
    if (leaf > cpuid_max)
        return false;
    asm volatile ("cpuid"
                  : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
                  : "a" (leaf), "c" (subleaf));
    return true;
}

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("out %1, al"  : : "a" (value), "Nd" (port) : "memory");
}

static inline void outw(uint16_t port, uint16_t value) {
    asm volatile ("out %1, ax"  : : "a" (value), "Nd" (port) : "memory");
}

static inline void outd(uint16_t port, uint32_t value) {
    asm volatile ("out %1, eax" : : "a" (value), "Nd" (port) : "memory");
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile ("in al, %1"  : "=a" (value) : "Nd" (port) : "memory");
    return value;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t value;
    asm volatile ("in ax, %1"  : "=a" (value) : "Nd" (port) : "memory");
    return value;
}

static inline uint32_t ind(uint16_t port) {
    uint32_t value;
    asm volatile ("in eax, %1" : "=a" (value) : "Nd" (port) : "memory");
    return value;
}

static inline void mmoutb(uintptr_t addr, uint8_t value) {
    asm volatile (
        "mov %0, %1"
        : "=m" (BYTE_PTR(addr))
        : "ir" (value)
        : "memory"
    );
}

static inline void mmoutw(uintptr_t addr, uint16_t value) {
    asm volatile (
        "mov %0, %1"
        : "=m" (WORD_PTR(addr))
        : "ir" (value)
        : "memory"
    );
}

static inline void mmoutd(uintptr_t addr, uint32_t value) {
    asm volatile (
        "mov %0, %1"
        : "=m" (DWORD_PTR(addr))
        : "ir" (value)
        : "memory"
    );
}

static inline void mmoutq(uintptr_t addr, uint64_t value) {
    asm volatile (
        "mov %0, %1"
        : "=m" (QWORD_PTR(addr))
        : "ir" (value)
        : "memory"
    );
}

static inline uint8_t mminb(uintptr_t addr) {
    uint8_t ret;
    asm volatile (
        "mov %0, %1"
        : "=r" (ret)
        : "m"  (BYTE_PTR(addr))
        : "memory"
    );
    return ret;
}

static inline uint16_t mminw(uintptr_t addr) {
    uint16_t ret;
    asm volatile (
        "mov %0, %1"
        : "=r" (ret)
        : "m"  (WORD_PTR(addr))
        : "memory"
    );
    return ret;
}

static inline uint32_t mmind(uintptr_t addr) {
    uint32_t ret;
    asm volatile (
        "mov %0, %1"
        : "=r" (ret)
        : "m"  (DWORD_PTR(addr))
        : "memory"
    );
    return ret;
}

static inline uint64_t mminq(uintptr_t addr) {
    uint64_t ret;
    asm volatile (
        "mov %0, %1"
        : "=r" (ret)
        : "m"  (QWORD_PTR(addr))
        : "memory"
    );
    return ret;
}

static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t edx, eax;
    asm volatile ("rdmsr"
                  : "=a" (eax), "=d" (edx)
                  : "c" (msr)
                  : "memory");
    return ((uint64_t)edx << 32) | eax;
}

static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t edx = value >> 32;
    uint32_t eax = (uint32_t)value;
    asm volatile ("wrmsr"
                  :
                  : "a" (eax), "d" (edx), "c" (msr)
                  : "memory");
}

#define write_cr(reg, val) ({ \
    asm volatile ("mov cr" reg ", %0" :: "r" (val) : "memory"); \
})

#define read_cr(reg) ({ \
    size_t cr; \
    asm volatile ("mov %0, cr" reg : "=r" (cr) :: "memory"); \
    cr; \
})

#define locked_read(var) ({ \
    typeof(*var) ret = 0; \
    asm volatile ( \
        "lock xadd %1, %0;" \
        : "+r" (ret) \
        : "m" (*(var)) \
        : "memory" \
    ); \
    ret; \
})

#define locked_write(var, val) ({ \
    typeof(*var) ret = val; \
    asm volatile ( \
        "lock xchg %1, %0;" \
        : "+r" ((ret)) \
        : "m" (*(var)) \
        : "memory" \
    ); \
    ret; \
})

#endif
