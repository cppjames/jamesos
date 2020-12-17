#ifndef _STRING_H
#define _STRING_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);
void* memcpy(void* __restrict, const void* __restrict, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);

char* strchr(const char*, char);
size_t strlen(const char*);

size_t itostr(uint8_t [], int32_t, size_t);

int32_t strtoi(const char [], size_t);
uint32_t strtou(const char [], size_t);

#ifdef __cplusplus
}
#endif

#endif // _STRING_H