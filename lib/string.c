/*
 * string.h - string operations
 * Conforming to IEEE Std 1003.1, 2004 Edition (SUSv3)
 */


#ifndef _STRING_C
#define _STRING_C

#include <string.h>
#include <stddef.h>  /* size_t, NULL */

void *memccpy(void *restrict dest, const void *restrict src, int c, size_t n)
{
	return NULL;
}

void *memchr(const void *s, int c, size_t n)
{
	return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	return 0;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
	int i;
	for(i = 0; i < n; i++) {
		*((char *)dest + i) = *((char *)src + i);
	}
	return dest;
}

void *memmove(void *dest, const void *src, size_t n)
{
	return NULL;
}

void *memset(void *s, int c, size_t n)
{
	int i;
	for(i = 0; i < n; i++) {
		*((char *)s + i) = c;
	}
	return s;
}

char *strcat(char *restrict dest, const char *restrict src)
{
	return NULL;
}

char *strchr(const char *s, int c)
{
	return NULL;
}

int strcmp(const char *s1, const char *s2)
{
	return 0;
}

int strcoll(const char *s1, const char *s2)
{
	return 0;
}

char *strcpy(char *restrict dest, const char *restrict src)
{
	return NULL;
}

size_t strcspn(const char *s, const char *reject)
{
	return 0;
}

char *strdup(const char *s)
{
	return NULL;
}

char *strerror(int errnum)
{
	return NULL;
}

int *strerror_r(int errnum, char *buf, size_t n)
{
	return NULL;
}

size_t strlen(const char *s)
{
	return 0;
}

char *strncat(char *restrict dest, const char *restrict src, size_t n)
{
	return NULL;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	return 0;
}

char *strncpy(char *restrict dest, const char *restrict src, size_t n)
{
	return NULL;
}

char *strpbrk(const char *s, const char *accept)
{
	return NULL;
}

char *strrchr(const char *s, int c)
{
	return NULL;
}

size_t strspn(const char *s, const char *accept)
{
	return 0;
}

char *strstr(const char *haystack, const char *needle)
{
	return NULL;
}

char *strtok(char *restrict s, const char *restrict delim)
{
	return NULL;
}

char *strtok_r(char *s, const char *delim, char **ptrptr)
{
	return NULL;
}

size_t strxfrm(char *restrict dest, const char *restrict src, size_t n)
{
	return 0;
}

#endif
