#ifndef _VIRTUALWND_H
#define _VIRTUALWND_H

#include "basewnd.h"

#define VIRTUALWND_PARENT BaseWnd
//CUT #define AUTOWH 0xFFFE
//CUT #define NOCHANGE 0xFFFD

class NOVTABLE VirtualWnd : public VIRTUALWND_PARENT {
protected:
  VirtualWnd();
public:
  virtual ~VirtualWnd();
  virtual int init(RootWnd *parent, int nochild=FALSE);
  virtual int init(OSMODULEHANDLE moduleHandle, OSWINDOWHANDLE parent, int nochild=FALSE);

  virtual void bringToFront();
  virtual void bringToBack();
  virtual void bringAbove(BaseWnd *w);
  virtual void bringBelow(BaseWnd *w);
  virtual int onMouseMove(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  virtual int onMiddleButtonUp(int x, int y);

  //NONPORTABLE--avoid prolonged use
  virtual OSWINDOWHANDLE getOsWindowHandle();
  virtual OSMODULEHANDLE getOsModuleHandle();

public:
  virtual void resize(int x, int y, int w, int h, int wantcb=1);
  virtual void resize(RECT *r, int wantcb=1);
  virtual void move(int x, int y);
  virtual void invalidate();
  virtual void invalidateRect(RECT *r);
  virtual void invalidateRgn(Region *reg);
  virtual void validate();
  virtual void validateRect(RECT *r);
  virtual void validateRgn(Region *reg);
  virtual void getClientRect(RECT *);
  virtual void getNonClientRect(RECT *);
  virtual void getWindowRect(RECT *);
  virtual int beginCapture();
  virtual int endCapture();
  virtual int getCapture();
  virtual void setVirtualChildCapture(BaseWnd *child);
  virtual void repaint();
/*  virtual int focusNextSibbling(int dochild);
  virtual int focusNextVirtualChild(BaseWnd *child);*/
  virtual int cascadeRepaint(int pack=1); 
  virtual int cascadeRepaintRect(RECT *r, int pack=1); 
  virtual int cascadeRepaintRgn(Region *r, int pack=1); 
  virtual RootWnd *rootWndFromPoint(POINT *pt);
  virtual double getRenderRatio();
  virtual int reparent(RootWnd *newparent);
  virtual int setVirtual(int i);
  virtual RootWnd *getRootParent();
  virtual int gotFocus();
  virtual int onGetFocus();
  virtual int onKillFocus();
  virtual void setFocus();
  virtual int onActivate();
  virtual int onDeactivate();
  virtual void setVirtualChildFocus(RootWnd *child);
  virtual int wantFocus() { return 0; }
  virtual void setAllowDeactivation(int allow);
  virtual int allowDeactivation();

public:
  virtual int isVirtual() { return !bypassvirtual; }

protected:
  int virtualX,virtualY,virtualH,virtualW;
  int bypassvirtual;
  int focus;
  int resizecount;
  double lastratio;
};

#endif
