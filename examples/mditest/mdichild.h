#ifndef MDICHILD_H
#define MDICHILD_H

#include <wnd/wnds/os/osmdichildwnd.h>

class OSFrameWnd;
class PopupMenu;
class MDIFrame;
class Bitmap;

#define MDICHILD_PARENT OSMDIChildWnd

class MDIChild : public MDICHILD_PARENT {
public:
  MDIChild(MDIFrame *parentframe, Bitmap *bmp);
  virtual ~MDIChild();
  virtual int onPaint(Canvas *c);
  virtual int onResize();
  virtual void onUserClose();
  virtual int onLeftButtonDblClk(int x, int y);

  virtual int getPreferences(int what);

private:
  MDIFrame *m_frame;
  Bitmap *m_bmp;
};

#endif
