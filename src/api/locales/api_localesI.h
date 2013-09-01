#ifndef __WASABI_APILOCALES_H_
#define __WASABI_APILOCALES_H_

#include <bfc/std.h>
#include <bfc/string/string.h>
#include <bfc/string/stringMap.h>

/*<?<autoheader/>*/
#include "api_locales.h"
#include "api_localesX.h"
/*?>*/
/*[interface.header.h]
#include <wnd/rootwnd.h>
*/

/*<?<classdecl name="api_locales" implname="api_localesI" factory="Interface" dispatchable="1"/>*/
class api_localesI : public api_localesX {
/*?>*/
public:
  NODISPATCH api_localesI() {}
  NODISPATCH ~api_localesI() {}

  static const char *getServiceName() { return "Locale Api"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

  DISPATCH(10) virtual void locales_addTranslation(const char *from, const char *to);
  DISPATCH(20) virtual const char * locales_getTranslation(const char *from);
  DISPATCH(30) virtual int locales_getNumEntries();
  DISPATCH(40) virtual const char * locales_enumEntry(int n);

  //dont know what to do with these yet
  //any insight from anyone on how to implement these would be good
  DISPATCH(50) const char *locales_getBindFromAction(int action) { return NULL; }
  DISPATCH(60) void locales_registerAcceleratorSection(const char *name, RootWnd *wnd, int global = 0) {}

private:
  NODISPATCH stringMap m_LocaleMap;
};

/*[interface.footer.h]
// {0928554A-72FA-438a-B5A8-CEE1115A8032}
static const GUID localesGuid = 
{ 0x928554a, 0x72fa, 0x438a, { 0xb5, 0xa8, 0xce, 0xe1, 0x11, 0x5a, 0x80, 0x32 } };

extern api_locales *localeApi;

*/

#endif //!__WASABI_APILOCALES_H_
