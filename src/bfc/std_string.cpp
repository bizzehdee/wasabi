#include "precomp.h"
#include "std_string.h"

#include "platform/platform.h"
#include "assert.h"

#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in std_string.cpp -- Things might be screwy till it's all debugged?")
#endif
# include <bfc/string/encodedstr.h>
#endif

char *DO_STRDUP(const char *ptr EXTRA_INFO) {
  char *ret;
  if (ptr == NULL) return NULL;
  ret = (char *)DO_MALLOC(STRLEN(ptr)+1, 0 EXTRA_PARAMS);
  if (ret != NULL) {
    STRCPY(ret, ptr);
  }
  return ret;
}

// Got bored trying to dup all my own 8's to 16's
wchar_t *DO_WSTRDUP(const char *ptr8 EXTRA_INFO) {
  wchar_t *ret;
  if (ptr8 == NULL) return NULL;
  int size8 = STRLEN(ptr8);
  ret = (wchar_t *)DO_MALLOC((size8 + 1) * sizeof(wchar_t), 1 EXTRA_PARAMS);
  if (ret != NULL) {
    int i;
    for (i = 0; i < size8; i++) {
      // thou shalt not sign extend.
      ret[i] = (ptr8[i]) & 0xFF;
    }
  }
  return ret;
}

#if 0
int TOUPPER(int c) {
  if (c >= 'a' && c <= 'z') c = (c - 'a') + 'A';
  return c;
}

int TOLOWER(int c) {
  if (c >= 'A' && c <= 'Z') c = (c - 'A') + 'a';
  return c;
}

int ISDIGIT(int c) {
  return (c >= '0' && c <= '9');
}

int ISALPHA(int c) {
  return (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z');
}
#endif

int STRLEN(const char *str) {
  ASSERT(str != NULL);
  return (int)strlen(str);
}

int STRCMP(const char *str1, const char *str2) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
  return strcmp(str1, str2);
}

int STRCMPSAFE(const char *str1, const char *str2, const char *defval1, const char *defval2) {
  if (str1 == NULL) str1 = defval1;
  if (str2 == NULL) str2 = defval2;
  return STRCMP(str1, str2);
}

int STRICMP(const char *str1, const char *str2) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
#if defined(WIN32)
  return lstrcmpi(str1, str2);
#elif defined(LINUX)
  return strcasecmp(str1, str2);
#endif
}

int STRICMPSAFE(const char *str1, const char *str2, const char *defval1, const char *defval2) {
  if (str1 == NULL) str1 = defval1;
  if (str2 == NULL) str2 = defval2;
  return STRICMP(str1, str2);
}

int STREQLSAFE(const char *str1, const char *str2, const char *defval1, const char *defval2) {
  return !STRCMPSAFE(str1, str2, defval1, defval2);
}

int STRCASEEQLSAFE(const char *str1, const char *str2, const char *defval1, const char *defval2) {
  return !STRICMPSAFE(str1, str2, defval1, defval2);
}

int STRNCMP(const char *str1, const char *str2, int l) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
  return strncmp(str1, str2, l);
}

int STRNNCMP(const char *str1, const char *str2) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
  return STRNCMP(str1, str2, STRLEN(str2));
}

//FG> sorry brennan, this need to not be a const :)
void STRTOUPPER(char *p) {
  while (*p) {
    *p = TOUPPER(*p);
    p++;
  }
}

void STRTOLOWER(char *p) {
  while (*p) {
    *p = TOLOWER(*p);
    p++;
  }
}

int STRNICMP(const char *str1, const char *str2, int l) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);

  ASSERT(l>=0);
  while (TOUPPER(*str1) == TOUPPER(*str2) && *str1 != 0 && *str2 != 0 && l--)
    str1++, str2++;
  if (l == 0) return 0;
  return (*str2 - *str1);
}

int STRNINCMP(const char *str1, const char *str2) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
  return STRNICMP(str1, str2, STRLEN(str2));
}

int STRNCASEEQL(const char *str1, const char *str2, int l) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
  return STRNICMP(str1, str2, l) == 0;
}

int STREQL(const char *str1, const char *str2) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
  return (strcmp(str1, str2) == 0);
}

int STRCASEEQL(const char *str1, const char *str2) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
#ifdef WIN32
  return (lstrcmpi(str1, str2)==0);
#else
  return (strcasecmp(str1, str2)==0);
#endif
}

int STRISNUM(const char *p) {
  for (; *p; p++)
    if (!ISDIGIT(*p)) return 0;
  return 1;
}

static int TOUPPERANDSLASH(int a) {
  if (a=='\\')
    a = '/';
  else
    a = TOUPPER(a);
  return a;
}

int PATHEQL(const char *str1, const char *str2) {
  if (str1 == NULL) {
    if (str2 == NULL) return TRUE;
    return FALSE;
  }
  while (TOUPPERANDSLASH(*str1) == TOUPPERANDSLASH(*str2) && *str1 != 0 && *str2 != 0) str1++, str2++;
  return *str1 == *str2;
}

char *STRSTR(const char *haystack, const char *needle) {
  ASSERT(haystack!=NULL);
  ASSERT(needle!=NULL);
  ASSERT(haystack != needle);
  return (char *)strstr(haystack, needle);
}

char *STRCASESTR(const char *str1, const char *str2) {
  ASSERT(str1!=NULL);
  ASSERT(str2!=NULL);
  char *p;
  int len = STRLEN(str2);
  for (p = (char *)str1; *p; p++) {
    if (STRNCASEEQL(p, str2, len)) return p;
  }
  return NULL;
}

char *STRCASESTRSAFE(const char *str1, const char *str2, const char *defval1, const char *defval2) {
  if (str1 == NULL) str1 = defval1;
  if (str2 == NULL) str2 = defval2;
  return STRCASESTR(str1, str2);
}

int STRCASERPL(char *s, const char *w, const char *r, int maxsize) {

  int rl = STRLEN(r);
  int wl = STRLEN(w);
  int gain = rl - wl;
  int i=0;
  char *o = s;

  while (1) {
    char *f = STRCASESTR(o, w);
    if (!f) break;
    int pos = (int)(s - f);
    if (pos + wl + gain >= maxsize) break;
    if (gain < 0) {
      MEMMOVE(f+wl+gain, f+wl, maxsize - pos - wl);
      s[maxsize-1] = 0;
    }
    else if (gain > 0) {
      MEMMOVE(f+wl+gain, f+wl, maxsize - pos - wl - gain);
      s[maxsize-1] = 0;
    }
    i++;
    MEMMOVE(f, r, rl);
    o = f+rl;
  }
  return i;
}

void STRCPY(char *dest, const char *src) {
  ASSERT(dest != NULL);
  ASSERT(src != NULL);
  ASSERT(dest != src);
  strcpy(dest, src);
//INLINE
}

void STRNCPY(char *dest, const char *src, int maxchar) {
  ASSERT(dest != NULL);
  ASSERT(src != NULL);
  strncpy(dest, src, maxchar);
//INLINE
}

char *STRCHR(const char *str, int c) {
  ASSERT(str != NULL);
  return (char*)strchr(str, c);//BU this C-style cast is gay, please use const_cast or static_cast, whichever is appropriate
//INLINE
}

char *STRRCHR(const char *str, int c) {
  ASSERT(str != NULL);
  const char *p = str+STRLEN(str)-1;
  while (*p && p >= str) {
    if (*p == c) return (char *)p;
    p--;
  }
  return NULL;
}

void STRCAT(char *dest, const char *append) {
  ASSERT(dest != NULL);
  ASSERT(append != NULL);
  ASSERT(dest != append);
  strcat(dest, append);
}

unsigned long STRTOUL(const char *s, char **p, int rx) {
  ASSERT(s != NULL);
  ASSERT(p != NULL);
  return strtoul(s,p,rx);
}

int WSTRLEN(const wchar_t *str16) {
  if (!str16) return 0;
#ifdef WIN32
  return lstrlenW(str16);
#else
  int r = 0;
  while (*str16++ != 0) r++;
  return r;
#endif
}


void WSTRCAT(wchar_t *dest, const wchar_t *append) {
  ASSERT(dest != NULL);
  ASSERT(append != NULL);
  ASSERT(dest != append);
#ifdef WIN32
  lstrcatW(dest, append);
#else
  DebugString( "portme -- Std, WSTRCAT\n" );
#ifdef PLATFORM_WARNINGS
#warning WSTRCAT not implemented on LINUX
#endif
#endif
}

void WSTRCAT7(wchar_t *dest, const char *append) {
  ASSERT(dest != NULL);
  ASSERT(append != NULL);
  wchar_t *dupe = WSTRDUP(append);
  WSTRCAT(dest, dupe);
  FREE(dupe);
}

int WSTRCMP(const wchar_t *a, const wchar_t *b) {
  ASSERT(a != NULL);
  ASSERT(b != NULL);
#ifdef WIN32
  return lstrcmpW(a, b);
#else
#ifdef PLATFORM_WARNINGS
#warning WSTRCMP not implemented on LINUX
#endif
  DebugString( "portme -- Std, WSTRCMP\n" );
  return 0;
#endif
}

int WSTRCMP7(const wchar_t *a, const char *b) {
  ASSERT(a != NULL);
  ASSERT(b != NULL);
  wchar_t *dupe = WSTRDUP(b);
  int retval = WSTRCMP(a, dupe);
  FREE(dupe);
  return retval;
}
