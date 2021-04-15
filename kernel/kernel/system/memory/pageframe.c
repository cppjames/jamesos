#include <kernel/system/memory.h>

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define FREE 0
#define USED 1
#define PAGE_SIZE 4096
#define MAX_PAGES 8192
#define FRAME_BUFFER_SIZE 16

uint8_t frame_map[MAX_PAGES / 8 + 1] = { 0 };

static inline void *getStartFrame() {
    size_t kernel_end_address = 0xFFFF800000118000;
    return (void*)((kernel_end_address & ~(size_t)0x0FFF) + PAGE_SIZE);
}

static inline bool getFrameStatus(size_t index) {
    return frame_map[index / 8] & (1 << (index % 8));
}

static inline void setFrameStatus(size_t index, bool status) {
    size_t bitmask = 1 << (index % 8);

    if (status)
        frame_map[index / 8] |= bitmask;
    else
        frame_map[index / 8] &= ~bitmask;
}

static void *kallocFrameFind()
{
    for (size_t i = 0; i < MAX_PAGES; i++) {
        if (getFrameStatus(i) == FREE) {
            setFrameStatus(i, USED);
            return getStartFrame() + (i * PAGE_SIZE);
        }
    }

    return 0;
}

void *kallocFrame() {
    static void *frame_buffer[FRAME_BUFFER_SIZE] = { 0 };
    static size_t frame_index = 0;

    if (frame_index == 0)
        for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
            frame_buffer[i] = kallocFrameFind();

    void *frame = frame_buffer[frame_index];

    for (size_t i = 0; i < PAGE_SIZE; i++)
        ((unsigned char*)frame)[i] = 0x0;

    frame_index = (frame_index + 1) % FRAME_BUFFER_SIZE;
    return frame;
}

void kfreeFrame(void *frame) {
    void *frame_offset = (void*)(frame - getStartFrame());
    size_t index = (size_t)frame_offset / PAGE_SIZE;

    setFrameStatus(index, FREE);
}