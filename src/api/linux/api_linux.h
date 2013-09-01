#ifndef _API_LINUX_H
#define _API_LINUX_H

#include <bfc/dispatch.h>
#include <bfc/common.h>
//CUT #include <bfc/nsguid.h>

class String;

// ----------------------------------------------------------------------------

class COMEXP api_linux: public Dispatchable {
  protected:
    api_linux() {}
    ~api_linux() {}
  public:
#ifdef XWINDOW
    virtual Display *linux_getDisplay();
    XContext linux_getContext();
#endif
    int linux_getIPCId();
#ifdef XWINDOW
    XShmSegmentInfo *linux_createXShm( int );
    void linux_destroyXShm( XShmSegmentInfo * );
    const XineramaScreenInfo *linux_getXineramaInfo(int *num);
#endif

  protected:
    enum {
#ifdef XWINDOW
      API_LINUX_GETDISPLAY = 10,
      API_LINUX_GETCONTEXT = 20,
#endif
      API_LINUX_GETIPCID = 30,
#ifdef XWINDOW
      API_LINUX_CREATEXSHM = 40,
      API_LINUX_DESTROYXSHM = 50,
      API_LINUX_GETXINERAMAINFO = 60,
#endif
    };
};

// ----------------------------------------------------------------------------

#ifdef XWINDOW
inline Display *api_linux::linux_getDisplay() {
  return _call(API_LINUX_GETDISPLAY, (Display *)NULL);
}

inline XContext api_linux::linux_getContext() {
  return _call(API_LINUX_GETCONTEXT, (XContext)0);
}
#endif

inline int api_linux::linux_getIPCId() {
  return _call(API_LINUX_GETIPCID, (int)-1);
}

#ifdef XWINDOW
inline XShmSegmentInfo *api_linux::linux_createXShm( int n ) {
  return _call(API_LINUX_CREATEXSHM, (XShmSegmentInfo *)NULL, n );
}

inline void api_linux::linux_destroyXShm( XShmSegmentInfo *shm ) {
  _voidcall(API_LINUX_DESTROYXSHM, shm );
}

inline const XineramaScreenInfo *api_linux::linux_getXineramaInfo(int *num) {
  _call(API_LINUX_GETXINERAMAINFO, num, (const XineramaScreenInfo *)NULL );
}
#endif

// ----------------------------------------------------------------------------


// {1A3DBFA6-292B-47b8-9558-80C088354450}
static const GUID linuxApiServiceGuid =
{ 0x1a3dbfa6, 0x292b, 0x47b8, { 0x95, 0x58, 0x80, 0xc0, 0x88, 0x35, 0x44, 0x50 } };

extern api_linux *linuxApi;

#endif
