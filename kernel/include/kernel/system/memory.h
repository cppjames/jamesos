#ifndef MEMORY_H
#define MEMORY_H

void init_paging();

void *kallocFrame();
void kfreeFrame(void *frame);

#endif // MEMORY_H