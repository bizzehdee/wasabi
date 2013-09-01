#ifndef _API_LINUXX_H
#define _API_LINUXX_H

#include "api_linux.h"


// ----------------------------------------------------------------------------

class COMEXP api_linuxX : public api_linux {
  protected:
    api_linuxX() {}
  public:

#ifdef XWINDOW
    virtual Display *linux_getDisplay()=0;
    virtual XContext linux_getContext()=0;
#endif
    virtual int linux_getIPCId()=0;
#ifdef XWINDOW
    virtual XShmSegmentInfo *linux_createXShm( int )=0;
    virtual void linux_destroyXShm( XShmSegmentInfo * )=0;
    virtual const XineramaScreenInfo *linux_getXineramaInfo(int *num)=0;
#endif

  protected:
    RECVS_DISPATCH;
};

#endif // __API_linuxX_H
