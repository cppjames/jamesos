#include <kernel/system/exceptions.h>
#include <stdint.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>
#include <kernel/kcontrol.h>

static char *exceptions[] = {
    [0] = "Division by Zero",
    [1] = "Debug",
    [2] = "Non Maskable Interrupt",
    [3] = "Breakpoint",
    [4] = "Overflow",
    [5] = "Bound Range Exceeded",
    [6] = "Invalid opcode",
    [7] = "Device not available",
    [8] = "Double Fault",
    [10] = "Invalid TSS",
    [11] = "Segment not present",
    [12] = "Stack Exception",
    [13] = "General Protection fault",
    [14] = "Page fault",
    [16] = "x87 Floating Point Exception",
    [17] = "Alignment check",
    [18] = "Machine check",
    [19] = "SIMD floating point Exception",
    [20] = "Virtualization Exception",
    [30] = "Security Exception"
};

void exception_handler(uint64_t exc) {
    klog_debug("Unhandled exception %d: %s\n", exc, exceptions[exc]);
    kpanic("Unhandled exception: %s", exceptions[exc]);
}

void exc0_handler() {
    exception_handler(0);
};

void exc1_handler() {
    exception_handler(1);
};

void exc2_handler() {
    exception_handler(2);
};

void exc3_handler() {
    exception_handler(3);
};

void exc4_handler() {
    exception_handler(4);
};

void exc5_handler() {
    exception_handler(5);
};

void exc6_handler() {
    exception_handler(6);
};

void exc7_handler() {
    exception_handler(7);
};

void exc8_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(8);
};

void exc9_handler() {
    exception_handler(9);
};

void exc10_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(10);
};

void exc11_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(11);
};

void exc12_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(12);
};

void exc13_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(13);
};

void exc14_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(14);
};

void exc15_handler() {
    exception_handler(15);
};

void exc16_handler() {
    exception_handler(16);
};

void exc17_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(17);
};

void exc18_handler() {
    exception_handler(18);
};

void exc19_handler() {
    exception_handler(19);
};

void exc20_handler() {
    exception_handler(20);
};

void exc30_handler(__attribute__((unused)) uint64_t error_code) {
    exception_handler(30);
};