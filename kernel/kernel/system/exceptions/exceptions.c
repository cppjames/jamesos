#include <kernel/system/exceptions.h>

#include <kernel/kcontrol.h>
#include <kernel/kdebug.h>
#include <kernel/kinfo.h>

#include <stdint.h>

#define defineExcHandler(exc) \
    void excHandler##exc(void) { exceptionHandler(exc); }

#define defineExcHandlerErrorCode(exc) \
    void excHandler##exc(uint64_t error_code) { (void)error_code; exceptionHandler(exc); }

static const char *exceptions[] = {
    [0]  = "Division by Zero",
    [1]  = "Debug",
    [2]  = "Non Maskable Interrupt",
    [3]  = "Breakpoint",
    [4]  = "Overflow",
    [5]  = "Bound Range Exceeded",
    [6]  = "Invalid opcode",
    [7]  = "Device not available",
    [8]  = "Double Fault",
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


void exceptionHandler(uint64_t exc) {
    kdebugLog("Unhandled exception %d: %s\n", exc, exceptions[exc]);
    kpanic("Unhandled exception: %s", exceptions[exc]);
}


defineExcHandler(0);
defineExcHandler(1);
defineExcHandler(2);
defineExcHandler(3);
defineExcHandler(4);
defineExcHandler(5);
defineExcHandler(6);
defineExcHandler(7);
defineExcHandlerErrorCode(8);
defineExcHandler(9);
defineExcHandlerErrorCode(10);
defineExcHandlerErrorCode(11);
defineExcHandlerErrorCode(12);
defineExcHandlerErrorCode(13);
defineExcHandlerErrorCode(14);
defineExcHandler(15);
defineExcHandler(16);
defineExcHandlerErrorCode(17);
defineExcHandler(18);
defineExcHandler(19);
defineExcHandler(20);
defineExcHandlerErrorCode(30);