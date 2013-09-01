#ifndef _WASABI_SCROLLBAR_H
#define _WASABI_SCROLLBAR_H

#include <wnd/virtualwnd.h>
#include <wnd/region.h>
#include <wnd/usermsg.h>

#include <draw/bitmap.h>

#define SCROLLBAR_FULL 65535

#define POS_NONE    0
#define POS_LEFT    1
#define POS_BUTTON  2
#define POS_RIGHT   3

#define PAGE_NONE   0
#define PAGE_DOWN   1
#define PAGE_UP     2

#define DEFAULT_HEIGHT 16


#define SCROLLBAR_PARENT VirtualWnd
class ScrollBar : public SCROLLBAR_PARENT {
public:
  ScrollBar();
  
  virtual ~ScrollBar();

  virtual int onMouseMove (int x, int y);
  
  virtual int onLeftButtonDown(int x, int y);
  virtual int onLeftButtonUp(int x, int y);
  
  virtual int onRightButtonDown(int x, int y);
  virtual int onRightButtonUp(int x, int y);
  
  virtual int onMouseWheelUp(int clicked, int lines);
  virtual int onMouseWheelDown(int clicked, int lines);
  
  virtual int onPaint(Canvas *canvas);
  
  virtual int onResize();
  
  virtual int onInit();
  
  virtual void timerCallback(int id);

  virtual int wantDoubleClicks() { return 0; };
  
  virtual int onSetPosition(BOOL smooth=FALSE);
  
  virtual int onSetFinalPosition();

  
  void setBitmaps(char *left, char *lpressed, char *lhilite,
                  char *right, char *rpressed, char *rhilite, 
                  char *button, char *bpressed, char *bhilite);
  
  void setBackgroundBitmaps(const char *left, const char *middle, const char *right);

  void setPosition(int pos);
  
  int getPosition();
  
  int getHeight();
  
  void setHeight(int newheight);
  
  void setNPages(int n);
  
  void gotoPage(int n);
  
  void setUpDownValue(int newupdown);
  
  void setVertical(BOOL isvertical);
  
  int getWidth();

  
  virtual void freeResources();
  
  virtual void reloadResources();

protected:
  virtual LRESULT wndProc(OSWINDOWHANDLE hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  
  void deleteResources();
  
  int getMousePosition();
  
  void calcOverlapping();
  
  void calcXPosition();
  
  void calcPosition();
  
  void handlePageUpDown();
  
  int checkPageUpDown();
  
  void handleUpDown();
  
  int checkUpDown();
  
  int pageUp();
  
  int pageDown();
  
  int upDown(int which);
  
  void setPrivatePosition(int pos, BOOL signal=TRUE, BOOL smooth=FALSE);
  
  void loadBmps();

  Bitmap bmpleft, bmplpressed, bmplhilite,
             bmpright, bmprpressed, bmprhilite,
             bmpbutton, bmpbpressed, bmpbhilite,
             bmpbackgroundleft, bmpbackgroundmiddle, bmpbackgroundright;

  RegionI *leftrgn, *rightrgn, *buttonrgn;
  int position;

  int moving;
  int lefting;
  int righting;
  int clicked;

  int buttonx;

  int curmouseposition;
  int clickmouseposition;
  int height;

  int shiftleft, shiftright;
  POINT clickpos;
  int clickbuttonx;
  int pageing;
  int firstdelay;
  int timer;
  int npages;
  int pageway;
  int updown;
  int timer2;
  int insetpos;

  int vertical;
  int lastx, lasty;
};

#endif
