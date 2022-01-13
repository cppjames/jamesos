#include <kernel/system/sse.h>
#include <kernel/system/vmm.h>

extern void setSseControlBits(void);

alignas(16) SseStateBuffer sseState;

void initSse(void) {
    setSseControlBits();
    // FIXME - Why does it throw #GP?
    // saveSseState(&sseState);
}