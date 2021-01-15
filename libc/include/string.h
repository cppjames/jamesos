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
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);
char* strncpy(char*, const char*, size_t);
size_t strlen(const char*);
size_t strcspn(const char*, const char*);
size_t strspn(const char*, const char*);
char* strstr(const char*, const char*);

size_t itostr(char [], int, size_t);
size_t ztostr(char [], size_t, size_t);

int strtoi(const char [], size_t);
unsigned int strtou(const char [], size_t);

#ifdef __cplusplus
}
#endif

#endif // _STRING_H