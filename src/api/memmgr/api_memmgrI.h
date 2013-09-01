#ifndef __API_MEMMGR_IMPL_H
#define __API_MEMMGR_IMPL_H

/*<?<autoheader/>*/
#include "api_memmgr.h"
#include "api_memmgrx.h"
/*?>*/
/*[interface.header.h]
*/

#include <api/service/services.h>
/*<?<classdecl name="api_memmgr" implname="api_memmgrI" factory="Interface" dispatchable="1"/>*/
class api_memmgrI : public api_memmgrX {
/*?>*/
public:
  NODISPATCH api_memmgrI();
  NODISPATCH virtual ~api_memmgrI();
  
  static const char *getServiceName() { return "Memory Manager API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

  DISPATCH(10) virtual void *sysMalloc(unsigned int size);
  DISPATCH(20) virtual void sysFree(void *ptr);
  DISPATCH(30) virtual void *sysReAlloc(void *ptr, unsigned int size);
  DISPATCH(40) virtual void *sysMemSet(void *ptr, int val, int size);
  DISPATCH(50) virtual void *sysMemZero(void *ptr, int size);
};

/*[interface.footer.h]
// {0EED260E-C428-4c19-A152-F75F190E0B07}
static const GUID memmgrApiServiceGuid= 
{ 0xeed260e, 0xc428, 0x4c19, { 0xa1, 0x52, 0xf7, 0x5f, 0x19, 0xe, 0xb, 0x7 } };

extern api_memmgr *memmgrApi;
*/

#endif // __API_MEMMGR_IMPL_H
