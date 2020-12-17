#ifndef __MM__MTRR_H__
#define __MM__MTRR_H__

#include <stdint.h>
#include <stdbool.h>

#define MTRR_MEMORY_TYPE_UC 0x00
#define MTRR_MEMORY_TYPE_WC 0x01
#define MTRR_MEMORY_TYPE_WT 0x04
#define MTRR_MEMORY_TYPE_WP 0x05
#define MTRR_MEMORY_TYPE_WB 0x06

void mtrr_save(void);
void mtrr_restore(void);
bool mtrr_set_range(uint64_t base, uint64_t size, uint8_t caching_type);

#endif
