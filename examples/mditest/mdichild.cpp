#include <precomp.h>
#include "mdichild.h"
#include "mdiframe.h"
#include <draw/bitmap.h>

MDIChild::MDIChild(MDIFrame *parentframe, Bitmap *bmp) : m_frame(parentframe) {
  ASSERT(m_frame);
  m_frame->registerChild(this);
  m_bmp = bmp;
}

MDIChild::~MDIChild() {
  delete m_bmp;
  m_frame->unregisterChild(this);
}

int MDIChild::onPaint(Canvas *c) {
  MDICHILD_PARENT::onPaint(c);
  RECT r;
  getClientRect(&r);
  m_bmp->stretchToRect(c, &r);
  return 1;
}

int MDIChild::onResize() {
  MDICHILD_PARENT::onResize();
  invalidate();
  return 1;
}

void MDIChild::onUserClose() {
  m_frame->postDeferredCallback(DC_CLOSECHILD, (int)this);
}

int MDIChild::getPreferences(int what) {
  switch (what) {
    case SUGGESTED_W: {
      RECT wr;
      getWindowRect(&wr);
      RECT cr;
      getClientRect(&cr);
      return m_bmp->getWidth()+((wr.right-wr.left)-(cr.right-cr.left));
    }
    case SUGGESTED_H: {
      RECT wr;
      getWindowRect(&wr);
      RECT cr;
      getClientRect(&cr);
      return m_bmp->getHeight()+((wr.bottom-wr.top)-(cr.bottom-cr.top));
    }
  }
  return MDICHILD_PARENT::getPreferences(what);
}

int MDIChild::onLeftButtonDblClk(int x, int y) {
  MDICHILD_PARENT::onLeftButtonDblClk(x, y);
  POINT pt;
  getPosition(&pt);
  resize(pt.x, pt.y, getPreferences(SUGGESTED_W), getPreferences(SUGGESTED_H));
  return 1;
}

