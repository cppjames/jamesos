#include <kernel/system/exceptions.h>
#include <stdint.h>

#include <kernel/kinfo.h>
#include <kernel/kdebug.h>
#include <kernel/kcontrol.h>

#define def_excHandler(exc) \
    void excHandler##exc() { exceptionHandler(exc); }

#define def_excHandlerErrorCode(exc) \
    void excHandler##exc(uint64_t error_code) { (void)error_code; exceptionHandler(exc); }

static const char *exceptions[] = {
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


void exceptionHandler(uint64_t exc) {
    kdebugLog("Unhandled exception %d: %s\n", exc, exceptions[exc]);
    kpanic("Unhandled exception: %s", exceptions[exc]);
}


def_excHandler(0);
def_excHandler(1);
def_excHandler(2);
def_excHandler(3);
def_excHandler(4);
def_excHandler(5);
def_excHandler(6);
def_excHandler(7);
def_excHandlerErrorCode(8);
def_excHandler(9);
def_excHandlerErrorCode(10);
def_excHandlerErrorCode(11);
def_excHandlerErrorCode(12);
def_excHandlerErrorCode(13);
def_excHandlerErrorCode(14);
def_excHandler(15);
def_excHandler(16);
def_excHandlerErrorCode(17);
def_excHandler(18);
def_excHandler(19);
def_excHandler(20);
def_excHandlerErrorCode(30);