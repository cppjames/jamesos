#ifndef SSE_H
#define SSE_H

#include <stdalign.h>

typedef struct SseStateBuffer {
    alignas(16) char buffer[512];
} SseStateBuffer;

void initSse(void);
void saveSseState(SseStateBuffer* buffer);

#endif // SSE_H