/*

  Nullsoft WASABI Source File License

  Copyright 1999-2003 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

*/
#include "precomp.h"
#include "paintcb.h"
#include "rootwnd.h"

#define CBCLASS PaintCallbackInfoI
START_DISPATCH;
  CB(PAINTCBINFO_GETCANVAS, getCanvas);
  CB(PAINTCBINFO_GETREGION, getRegion);
END_DISPATCH;

PaintCallback::PaintCallback(RootWnd *w) {
  monitorWindow(w);
}

PaintCallback::~PaintCallback() {
  if (wnd != NULL) viewer_delViewItem(wnd);
}

void PaintCallback::monitorWindow(RootWnd *w) {
  if (wnd != NULL) {
    viewer_delViewItem(wnd);
    wnd = NULL;
  }
  if (w != NULL) {
    viewer_addViewItem(w);
    wnd = w;
  }
}

int PaintCallback::viewer_onItemDeleted(RootWnd *item) {
  ASSERT(item == wnd);//jic
  onWindowDeleted(wnd);
  wnd = NULL;
  return 1;
}

int PaintCallback::viewer_onEvent(RootWnd *item, int event, int param, void *ptr, int ptrlen) {
  PaintCallbackInfo *info = reinterpret_cast<PaintCallbackInfo *>(ptr);
  switch (event) {
    case RootWnd::Event_ONPAINT:
      if (param == BEFOREPAINT)
        onBeforePaint(info);
      else
        onAfterPaint(info);
    break;
    case RootWnd::Event_ONINVALIDATE:
      onInvalidation(info);
    break;
  }
  return 1;
}

