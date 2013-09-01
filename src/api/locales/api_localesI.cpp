#include <precomp.h>
//<?#include "<class data="implementationheader"/>"
#include "api_localesI.h"
//?>

api_locales *localeApi = NULL;

void api_localesI::locales_addTranslation(const char *from, const char *to) {
  //we dont want to be able to translate to or from NULL, its bad...
  if(from == NULL) return;
  if(to == NULL) return;

  m_LocaleMap.addPair(from, to);
}

const char * api_localesI::locales_getTranslation(const char *from) {
  //we dont allow null's to be mapped remember :)
  if(from == NULL) return "from cant be null";

  const char *r = m_LocaleMap.getMapped(from);
  if(r) return r;

  //if this string doesnt exist, then return from
  return from;
}

int api_localesI::locales_getNumEntries() {
  return m_LocaleMap.getNumPairs();
}

const char * api_localesI::locales_enumEntry(int n) {
  if(n < 0 && n >= m_LocaleMap.getNumPairs()) return "out of bounds";

  return m_LocaleMap.enumMapped(n);
}
