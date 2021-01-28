#include <kernel/kdebug.h>
#include <sys/io.h>
#include <stdint.h>
#include <stdbool.h>

void printch_port(char ch, __attribute__((unused)) void* arg) {
    outb(0xE9, (uint8_t)ch);
}