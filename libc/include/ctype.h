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

inline bool iscntrl(char ch) {
	return (ch >= 0x00 && ch <= 0x1F) || (ch == 0x7F);
}

inline bool isprint(char ch) {
	return (ch >= 0x20 && ch <= 0x7E);
}

inline bool isgraph(char ch) {
	return (ch >= 0x21 && ch <= 0x7E);
}

inline bool isxdigit(char ch) {
    return (ch >= 'A' && ch <= 'F') ||
           (ch >= 'a' && ch <= 'f') ||
	       isdigit(ch);
}

inline bool isblank(char ch) {
	return (ch == ' ' || ch == '\t');
}

inline bool ispunct(char ch) {
	return (ch >= 0x21 && ch <= 0x2F) ||
	       (ch >= 0x3A && ch <= 0x40) ||
	       (ch >= 0x5B && ch <= 0x60) ||
           (ch >= 0x7B && ch <= 0x7E);
}

#ifdef __cplusplus
}
#endif

#endif // _CTYPE_H