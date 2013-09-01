#ifndef _WASABI_BACKBUFFERWND_H
#define _WASABI_BACKBUFFERWND_H

//CUT #include "../autobitmap.h"

#if 0//BU
#ifdef WASABI_COMPILE_SKIN
#include "abstractwndhold.h"
#define BBWND_PARENT AbstractWndHolder
#else
#define BBWND_PARENT ServiceWndHolder
#endif
#else
#include <wnd/dockhostwnd.h>
#define BBWND_PARENT DockHostWnd
#endif

class NOVTABLE BackBufferWnd : public BBWND_PARENT {
protected:
  BackBufferWnd();
public:
  virtual ~BackBufferWnd();

  virtual int onPaint(Canvas *c); 

  virtual int wantBackBuffer() { return 0; }
  virtual BltCanvas *getBackBuffer();
  virtual int onSiblingInvalidateRgn(Region *r, RootWnd *who, int who_idx, int my_idx);
    
  virtual int wantSiblingInvalidations() { return wantBackBuffer(); }

private:
  int backbuffer;
  BltCanvas *back_buffer;
  int canvas_w, canvas_h;
};

#endif
