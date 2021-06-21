#include <sys/io.h>
#include <stdint.h>

uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %0, %1"
        : "=a"(ret)
        : "Nd"(port));
    return ret;
}

void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %1, %0"
        :
        : "a"(value), "Nd"(port)
        :);
}