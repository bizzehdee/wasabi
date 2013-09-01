#ifndef __LINUXAPI_STATIC_H
#define __LINUXAPI_STATIC_H

#include <bfc/platform/platform.h>

class LinuxAPI {
 public:
  static int init();
  static void kill();
#ifdef XWINDOW
  static Display *getDisplay();
  static XContext getContext();
#endif
  static DWORD getStarttime();
  static void setIPCId( int id );
  static int getIPCId( void );
#ifdef XWINDOW
  static XShmSegmentInfo *createXShm( int );
  static void destroyXShm( XShmSegmentInfo * );
  static const XineramaScreenInfo *getXineramaInfo( int *num );
#endif
};

#endif
