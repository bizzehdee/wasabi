#ifndef _STD_STRING_H
#define _STD_STRING_H

#include "std_mem.h"

static __inline int TOUPPER(int c) { return toupper(c); }
static __inline int TOLOWER(int c) { return tolower(c); }
static __inline int ISDIGIT(int c) { return isdigit(c); }
static __inline int ISALPHA(int c) { return isalpha(c); }
static __inline int ISSPACE(int c) { return isspace(c); }
static __inline int ISPUNCT(int c) { return ispunct(c); }
static __inline int ATOI(const char *str) { if (!str) return 0; return atoi(str); }
static __inline double ATOF(const char *str) { if (!str) return 0.0; return atof(str); }
static __inline int STRTOL(const char *str, char **stopstr, int base) {
  return strtol(str, stopstr, base);
}
// hex to int
static __inline int XTOI(const char *str) {
  char *dummy;
  return STRTOL(str, &dummy, 16);
}

#define MAKESAFE(x) ((x)?(x):"")

char *DO_STRDUP(const char *ptr EXTRA_INFO);
int STRLEN(const char *str);
int STRCMP(const char *str1, const char *str2);
int STRICMP(const char *str1, const char *str2);
int STRNCMP(const char *str1, const char *str2, int len);
int STRNNCMP(const char *str1, const char *str2); // uses len of str2
int STRNICMP(const char *str1, const char *str2, int len);
int STRNINCMP(const char *str1, const char *str2); // uses len of str2
int STREQL(const char *str1, const char *str2);
int STRCASEEQL(const char *str1, const char *str2);
char *STRCASESTR(const char *str1, const char *str2);
char *STRSTR(const char *haystack, const char *needle);
int STRCASERPL(char *s, const char *w, const char *r, int maxchar);
void STRCPY(char *dest, const char *src);
void STRNCPY(char *dest, const char *src, int maxchar);
char *STRCHR(const char *str, int c);
char *STRRCHR(const char *str, int c);
void STRCAT(char *dest, const char *append);
unsigned long STRTOUL(const char *str, char **p, int radix);

#ifdef __cplusplus
int STRCMPSAFE(const char *str1, const char *str2, const char *defval1="", const char *defval2="");
int STRICMPSAFE(const char *str1, const char *str2, const char *defval1="", const char *defval2="");
int STREQLSAFE(const char *str1, const char *str2, const char *defval1="", const char *defval2="");
int STRCASEEQLSAFE(const char *str1, const char *str2, const char *defval1="", const char *defval2="");
char *STRCASESTRSAFE(const char *str1, const char *str2, const char *defval1="", const char *defval2="");
#endif

int PATHEQL(const char *str1, const char *str2);
void STRTOUPPER(char *str);
void STRTOLOWER(char *str);
int STRISNUM(const char *p);

// Bare-bones utf16 support.  Just transform to utf8 as soon as possible,
// However, these are to make your life somewhat easier.
int WSTRLEN(const wchar_t *str16);
wchar_t *DO_WSTRDUP(const char *str8 EXTRA_INFO);
void WSTRCAT(wchar_t *dest, const wchar_t *append);
void WSTRCAT7(wchar_t *dest, const char *append);
int WSTRCMP(const wchar_t *a, const wchar_t *b);
int WSTRCMP7(const wchar_t *a, const char *b);

// do NOT define SPRINTF to be wsprintf. It is wayyy less functional than
// the real sprintf.
#define SPRINTF sprintf
#define SSCANF sscanf


#endif//_STD_STRING_H
