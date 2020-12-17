#ifndef _CTYPE_H
#define _CTYPE_H 1

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

inline bool isdigit(char c) {
	return c >= '0' && c <= '9';
}

inline bool islower(char c) {
	return c >= 'a' && c <= 'z';
}

inline bool isupper(char c) {
	return c >= 'A' && c <= 'Z';
}

inline bool isalpha(char c) {
	return islower(c) || isupper(c);
}

inline bool isalnum(char c) {
	return isdigit(c) || isalpha(c);
}

inline bool isspace(char c) {
	return c == ' ' 	||
		   c == '\f'	||
		   c == '\r'	||
		   c == '\t'	||
		   c == '\v';
}

inline bool tolower(char c) {
	return isupper(c) ? (c + ('a' - 'A')) : c;
}

inline bool toupper(char c) {
	return islower(c) ? (c + ('A' - 'a')) : c;
}

#ifdef __cplusplus
}
#endif

#endif // _CTYPE_H