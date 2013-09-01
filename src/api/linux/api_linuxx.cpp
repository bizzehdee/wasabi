#include <precomp.h>

#include "api_linuxx.h"

#ifdef CBCLASS
#undef CBCLASS
#endif

#define CBCLASS api_linuxX
  START_DISPATCH;
#ifdef XWINDOW
  CB(API_LINUX_GETDISPLAY, linux_getDisplay);
  CB(API_LINUX_GETCONTEXT, linux_getContext);
#endif
  CB(API_LINUX_GETIPCID, linux_getIPCId);
#ifdef XWINDOW
  CB(API_LINUX_CREATEXSHM, linux_createXShm);
  VCB(API_LINUX_DESTROYXSHM, linux_destroyXShm);
  CB(API_LINUX_GETXINERAMAINFO, linux_getXineramaInfo);
#endif
END_DISPATCH;
#undef CBCLASS
