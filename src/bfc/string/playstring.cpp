#include <precomp.h>

#include "playstring.h"

#include <api/api.h>

#define USE_TABLE

Playstring::Playstring(const char *_val) {
  val = NULL;
  setValue(_val);
}

Playstring::Playstring(const Playstring &ps) {
  val = NULL;
  setValue(ps.getValue());
}

Playstring::~Playstring() {
  setValue(NULL);
}

void Playstring::setValue(const char *newval) {
  _setValue(newval, 0);
}

void Playstring::_setValue(const char *newval, int tablenum) {
  ASSERT((int)newval != 0xdddddddd);
#ifdef USE_TABLE
#ifdef WASABI_COMPILE_METADB
  if (val != NULL) WASABI_API_METADB->metadb_releasePlaystring(val, tablenum);
#else
  FREE((void*)val);
#endif
#else
  FREE((void*)val);
#endif

  val = NULL;

  if (newval != NULL /*&& *newval != 0*/) {
#ifdef USE_TABLE
#ifdef WASABI_COMPILE_METADB
    val = WASABI_API_METADB->metadb_dupPlaystring(newval, tablenum);
#else
    val = STRDUP(newval);
#endif
#else
    val = STRDUP(newval);
#endif
  }
}

Playstring& Playstring::operator =(const Playstring &ps) {
  if (this != &ps) {
    setValue(ps.getValue());
  }
  return *this;
}
