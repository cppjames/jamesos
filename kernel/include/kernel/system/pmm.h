#ifndef PMM_H
#define PMM_H

#include "memory.h"

void initPmm();

void *kallocFrame();
void kfreeFrame(void *frame);

#endif // PMM_H