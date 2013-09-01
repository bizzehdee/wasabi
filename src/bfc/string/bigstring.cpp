#include <precomp.h>
#include "bigstring.h"

BigString::BigString(const char *initial_val) {
  mem = NULL;
  memsize = 0;
  m_linecount = 0;

  cat(initial_val);
}

BigString::~BigString() {
  if (mem != NULL) {
    FREE(mem);
  }
  strings.deleteAll();
}

const char *BigString::getValue() /*const*/ {
  if (mem != NULL) return mem;
  memsize = 0;
  foreach(strings)
    memsize += strings.getfor()->len();
  endfor;
  mem = (char *)MALLOC(memsize+1);
  char *p = mem;
  String *s = NULL;
  int sl = 0;
  foreach(strings)
    s = strings.getfor();
    sl = s->len();
    if (sl > 0) MEMCPY((void *)p, (void *)s->getValue(), sl);
    p += sl;
  endfor;
  *p = 0;
  return mem;
}

int BigString::len() {
  if (mem) return memsize;
  getValue();
  if (mem) return memsize;
  return STRLEN(getValue());	// dammit
}

const char *BigString::getValueSafe(const char *def_val) {
  const char *ret = getValue();
  if (ret == NULL) return def_val;
  return ret;
}

void BigString::setValue(const char *val) {
  if (mem != NULL) {
    FREE(mem);
    mem = NULL;
  }
  strings.deleteAll();
  cat(val);
}

int BigString::isempty() {
  if (strings.getNumItems() == 0) return 1;
  foreach(strings)
    if (!strings.getfor()->isempty()) return 0;
  endfor;
  return 1;
}

void BigString::reset() {
  FREE(mem); mem = NULL;
  strings.deleteAll();
  m_linecount = 0;
  memsize = 0;
}

void BigString::catn(const char *s, int n) {
  String str;
  str.catn(s, n);
  cat(str);
}

void BigString::cat(const char *s) {
  if (s == NULL || *s == '\0') return;
  FREE(mem); mem = NULL;
  char *p = (char *)s;
  while (p && *p) {
    if (*p == '\r' || *p == '\n') {
      if (*(p+1) == '\n' && *p == '\r') p++;
      m_linecount++;
    }
    p++;
  }
  strings.addItem(new String(s));
}

char BigString::lastChar() {
  return strings.getLast()->lastChar();
}

char BigString::firstChar() {
  const char *s = strings.getFirst()->getValue();
  return s ? *s : 0;
}

int BigString::getLineCount() {
  return m_linecount;
}

int BigString::replace(const char *find, const char *replace) {
  int ret=0;
  for (int i = 0; i < strings.n(); i++) {
    ret += strings.q(i)->replace(find, replace);
  }
  return ret;
}
