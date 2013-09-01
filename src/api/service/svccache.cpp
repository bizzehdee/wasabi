#include <precomp.h>
#include "svccache.h"
#include <bfc/string/string.h>

#if !defined(WASABI_API_SVC)
#error no no
#endif

SvcCache::SvcCache(GUID type) {
  int p = 0;
  list.setAutoSort(FALSE);
  for (;;) {
    waServiceFactory *svc = WASABI_API_SVC->service_enumService(type, p++);
    if (svc == NULL) break;
    const char *teststr = svc->getTestString();
    if (teststr == NULL || *teststr == '\0') continue;
//CUT DebugString("cachin %s\n", teststr);
    list.addItem(svc);
  }
  list.setAutoSort(TRUE);
}

waServiceFactory *SvcCache::findServiceFactory(const char *searchval) {
  return list.findItem(searchval);
}

int SvcCache::waServiceFactoryCompare::compareItem(waServiceFactory *p1, waServiceFactory* p2) {
  int r = STRICMP(p1->getTestString(), p2->getTestString());
  if (r == 0) return CMP3(p1, p2);
  return r;
}

int SvcCache::waServiceFactoryCompare::compareAttrib(const char *attrib, waServiceFactory *item) {
  return STRICMP(attrib, item->getTestString());
}
