#include <string.h>

int strcmp(const char* lhs, const char* rhs) {
	const unsigned char* s1 = (const unsigned char*)lhs;
	const unsigned char* s2 = (const unsigned char*)rhs;

    unsigned char c1 = { 0 };
    unsigned char c2 = { 0 };
    
	do {
		c1 = (unsigned char) *s1++;
		c2 = (unsigned char) *s2++;
		if (c1 == '\0') break;
	} while (c1 == c2);
	return c1 - c2;	
}