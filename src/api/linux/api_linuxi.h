#ifndef _API_LINUXI_H
#define _API_LINUXI_H

#include <wasabicfg.h>
#include "api_linux.h"
#include "api_linuxx.h"

#include <api/service/services.h>

class api_linuxI : public api_linuxX {
public:
  NODISPATCH api_linuxI();
  NODISPATCH virtual ~api_linuxI();

  static const char *getServiceName() { return "Linux API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

#ifdef XWINDOW
  DISPATCH(10) virtual Display *linux_getDisplay();
  DISPATCH(20) virtual XContext linux_getContext();
#endif
  DISPATCH(30) virtual int linux_getIPCId();
#ifdef XWINDOW
  DISPATCH(40) virtual XShmSegmentInfo *linux_createXShm( int );
  DISPATCH(50) virtual void linux_destroyXShm( XShmSegmentInfo * );
  DISPATCH(60) virtual const XineramaScreenInfo *linux_getXineramaInfo(int *num);
#endif
};

/*[interface.footer.h]
// {1A3DBFA6-292B-47b8-9558-80C088354450}
static const GUID linuxApiServiceGuid =
{ 0x1a3dbfa6, 0x292b, 0x47b8, { 0x95, 0x58, 0x80, 0xc0, 0x88, 0x35, 0x44, 0x50 } };

extern api_linux *linuxApi;
*/

#endif
