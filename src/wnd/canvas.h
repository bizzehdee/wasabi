//NONPORTABLE: the interface is portable, but the implementation sure isn't
#ifndef _CANVAS_H
#define _CANVAS_H

#include <wasabicfg.h>

#ifdef WIN32
#include <ddraw.h>
#endif

class Canvas;
class MemCanvasBmp;
class BaseWnd;
class RootWnd;
class Region;
class SkinBitmap;
class BltCanvas;
class Blender;

#include <bfc/stack.h>
#include <bfc/ptrlist.h>
#include <bfc/string/string.h>
#include <bfc/dispatch.h>

enum {
#ifdef WIN32
  PENSTYLE_SOLID=PS_SOLID,
  PENSTYLE_DASH=PS_DASH,
  PENSTYLE_DOT=PS_DOT,
#else
  PENSTYLE_SOLID=LineSolid,
  PENSTYLE_DASH=LineDoubleDash,
  PENSTYLE_DOT=LineDoubleDash,
#endif
};

// abstract base class: safe to use in API
class NOVTABLE CanvasBase : public Dispatchable {
protected:
  CanvasBase() {} // protect constructor

public:
  HDC getHDC() {
    return _call(GETHDC, (HDC)0);
  }
  RootWnd *getRootWnd() {
    return _call(GETROOTWND, (RootWnd*)0);
  }
  void *getBits() {
    return _call(GETBITS, (void *)0);
  }
  void getOffsets(int *x, int *y) {
    _voidcall(GETOFFSETS, x, y);
  }
  BOOL isFixedCoords() {//FG> allows onPaint to handle double buffers as well as normal DCs
    return _call(ISFIXEDCOORDS, FALSE);
  }
  BOOL getDim(int *w, int *h=NULL, int *p=NULL) { // w & h in pixels, pitch in bytes. 0 on success. 
    return _call(GETDIM, FALSE, w, h, p);
  }
  int getClipBox(RECT *r) { // returns 0 if no clipping region
    return _call(GETCLIPBOX, 0, r);
  }

  const char *getTextFont() {
    return _call(GETTEXTFONT, "");
  }
  int getTextSize() {
    return _call(GETTEXTSIZE, -1);
  }
  int getTextBold() {
    return _call(GETTEXTBOLD, 0);
  }
  int getTextAntialias() {
    return _call(GETTEXTAA, 0);
  }
  int getTextOpaque() {
    return _call(GETTEXTOPAQUE, 0);
  }
  int getTextUnderline() {
    return _call(GETTEXTUNDERLINE, 0);
  }
  int getTextItalic() {
    return _call(GETTEXTITALIC, 0);
  }
  int getTextAlign() {
    return _call(GETTEXTALIGN, -1);
  }
  COLORREF getTextColor() {
    return _call(GETTEXTCOLOR, RGB(0,0,0));
  }
  COLORREF getTextBkColor() {
    return _call(GETTEXTBKCOLOR, RGB(255,255,255));
  }
  int getTextAngle() {
    return _call(GETTEXTANGLE, 0);
  }

  enum {
    GETHDC		= 100,
    GETROOTWND		= 200,
    GETBITS		= 300,
    GETOFFSETS		= 400,
    ISFIXEDCOORDS	= 500,
    GETDIM		= 600,
    GETTEXTFONT		= 700,
    GETTEXTSIZE		= 710,
    GETTEXTBOLD		= 720,
    GETTEXTOPAQUE	= 730,
    GETTEXTALIGN	= 740,
    GETTEXTCOLOR	= 750,
    GETTEXTBKCOLOR	= 760,
    GETTEXTAA     	= 770,
    GETTEXTUNDERLINE	= 780,
    GETTEXTITALIC	= 790,
    GETCLIPBOX		= 800,
    GETTEXTANGLE  = 900,
  };
};

#ifdef PERSISTENT_STRETCH_TEMP_CANVAS
extern THREADSTORAGE BltCanvas *blit_tmpcanvas;
#endif

typedef struct {
  int style;
  int width;
  COLORREF color;
#ifdef WIN32
  HPEN hpen;
#endif
} penstruct;

class NOVTABLE Canvas : public CanvasBase {
protected:
  Canvas();
public:
  virtual ~Canvas();

// CanvasBase stuff
  HDC getHDC();
  RootWnd *getRootWnd();
  void *getBits();
  void getOffsets(int *x, int *y);
  BOOL isFixedCoords();
  BOOL getDim(int *w, int *h, int *p);
  void setBaseWnd(BaseWnd *b);
// end CanvasBase stuff

  virtual BaseWnd *getBaseWnd();

  // graphics commands
  void fillRect(const RECT *r, COLORREF color, int alpha=-1);
  void drawRect(const RECT *r, int solid, COLORREF color, int alpha=-1);
  void drawSunkenRect(const RECT *r, COLORREF hilitecolor, COLORREF shadowcolor);
  
  void fillRegion(Region *rgn, COLORREF color);

  // text commands
  void setTextFont(const char *font_id_name);
  const char *getTextFont();
  void pushTextFont(const char *font_id_name);
  const char *popTextFont();

  void setTextSize(int points);
  int getTextSize();
  void pushTextSize(int points);
  int popTextSize();

  void setTextBold(int bold);
  int getTextBold();
  void pushTextBold(int bold);
  int popTextBold();

  void setTextAntialias(int aa);
  int getTextAntialias();
  void pushTextAntialias(int aa);
  int popTextAntialias();

  void setTextOpaque(int op);
  int getTextOpaque();
  void pushTextOpaque(int op);
  int popTextOpaque();

  void setTextUnderline(int underlined);
  int getTextUnderline();
  void pushTextUnderline(int underlined);
  int popTextUnderline();

  void setTextItalic(int italicized);
  int getTextItalic();
  void pushTextItalic(int italicized);
  int popTextItalic();

  void setTextAlign(int al);
  int getTextAlign();
  void pushTextAlign(int al);
  int popTextAlign();

  void setTextColor(COLORREF color);
  COLORREF getTextColor();
  void pushTextColor(COLORREF color);
  COLORREF popTextColor();

  void setTextBkColor(COLORREF color);
  COLORREF getTextBkColor();
  void pushTextBkColor(COLORREF color);
  COLORREF popTextBkColor();

  void pushPen(COLORREF color);
  void pushPen(int style, int width, COLORREF color);
  void popPen();

  int getPenStyle();
  COLORREF getPenColor();
  int getPenWidth();

  int getTextAngle();
  void setTextAngle(int angle);

//CUT #ifdef WASABI_COMPILE_FONTS
  // normal text
  void textOut(int x, int y, const char *txt);
  void textOut(int x, int y, int w, int h, const char *txt);
  void textOutEllipsed(int x, int y, int w, int h, const char *txt);
  // returns height used
  void textOutWrapped(int x, int y, int w, int h, const char *txt);
  void textOutWrappedPathed(int x, int y, int w, const char *txt);
  void textOutCentered(const RECT *r, const char *txt);

  int getTextWidth(const char *text);
  int getTextHeight(const char *text);
  void getTextExtent(const char *text, int *w, int *h);
  int getTextHeight() { return getTextHeight("M"); }
//CUT #endif

  void selectClipRgn(Region *r);
  int getClipBox(RECT *r); // returns 0 if no clipping region
  int getClipRgn(Region *r); // returns 0 if no clipping region

      // Deprecated?
      void moveTo(int x, int y);
      void lineTo(int x, int y);

  void lineDraw(int fromX, int fromY, int toX, int toY);

  void drawSysObject(const RECT *r, int sysobj, int alpha=255);
  
  // alpha = -1 will not handle alpha at all (blit), use 255 to blend
  void __fastcall blit(int srcx, int srcy, CanvasBase *dest, int dstx, int dsty, int dstw, int dsth, int alpha=-1, Blender *blender=NULL);
  void __fastcall stretchblit_hires(float srcx, float srcy, float srcw, float srch, CanvasBase *dstcanvas, float dstx, float dsty, float dstw, float dsth, int alpha=-1, Blender *blender=NULL); 
  void __fastcall shiftblit(int srcx, int srcy, int srcw, int srch, CanvasBase *dstcanvas, float dstx, float dsty, int alpha=-1, Blender *blender=NULL); 
  // src* are in 16.16 fixed point
  void __fastcall stretchblit(int srcx, int srcy, int srcw, int srch, CanvasBase *dest, int dstx, int dsty, int dstw, int dsth, int alpha=-1, Blender *blender=NULL);
  
  void antiAliasTo(Canvas *dest, int w, int h, int aafactor);

  int getXOffset() const { return xoffset; }
  int getYOffset() const { return yoffset; }
  void offsetRect(RECT *r);
  void debug();

  void colorToColor(COLORREF from, COLORREF to, RECT *r);
  static double getSystemFontScale();

  static void premultiply(ARGB32 *m_pBits, int nwords, int newalpha=-1);
  static void demultiply(ARGB32 *bits, int nwords);
  void demultiplyRect(RECT r);

  static BltCanvas *makeTempCanvas(int w, int h, BltCanvas **staticcanvas=NULL, int extendedby=50);
  static void releaseTempCanvas(BltCanvas *canvas);

protected:
//CUT  virtual int _dispatch(int msg, void *retval, void **params=NULL, int nparam=0);
  RECVS_DISPATCH;

  HDC hdc;
  void *bits;
  int width, height, pitch;
  BOOL fcoord;
  int xoffset, yoffset;
  BaseWnd *srcwnd;

private:
  Stack<String*> fontstack;
  Stack<COLORREF> colorstack;
  Stack<COLORREF> bkcolorstack;
  Stack<penstruct> penstack;
  Stack<int> boldstack;
  Stack<int> aastack;
  Stack<int> opstack;
  Stack<int> unstack;
  Stack<int> itstack;
  Stack<int> alstack;
  Stack<int> sizestack;

/*  HFONT makeFont(int size, const char *fontfacename=NULL, int bold=FALSE);
  void deleteFont(HFONT font);*/

  int tsize;
  int align;
  int tbold, taa, topaque, tunderline, titalic;
  int penstyle;
  COLORREF pencolor;
  int penwidth;
  COLORREF tcolor;
  COLORREF bktcolor;
  String *tfont;
  int textangle;
#ifdef WIN32
  HPEN defpen;
  HPEN curpen;
#endif
#ifdef LINUX
  int raster_x, raster_y;
#endif

  static char retfontname[256];
};

namespace DrawSysObj {
  enum {
    BUTTON, BUTTON_PUSHED, BUTTON_DISABLED,
    OSBUTTON, OSBUTTON_PUSHED, OSBUTTON_DISABLED,
    OSBUTTON_CLOSE, OSBUTTON_CLOSE_PUSHED, OSBUTTON_CLOSE_DISABLED,
    OSBUTTON_MINIMIZE, OSBUTTON_MINIMIZE_PUSHED, OSBUTTON_MINIMIZE_DISABLED,
    OSBUTTON_MAXIMIZE, OSBUTTON_MAXIMIZE_PUSHED, OSBUTTON_MAXIMIZE_DISABLED,
  };
};

class WndCanvas : public Canvas {
public:
  WndCanvas();
  virtual ~WndCanvas();

  // address client area
  int attachToClient(BaseWnd *basewnd);
//CUT  // address entire window
//CUT  int attachToWnd(HWND _hWnd);	// NONPORTABLE: avoid! mostly for mainwnd
  
private:
  OSWINDOWHANDLE hWnd;
};

class PaintCanvas : public Canvas {
public:
  PaintCanvas();
  virtual ~PaintCanvas();

  int beginPaint(BaseWnd *basewnd);
  int beginPaint(OSWINDOWHANDLE wnd);
  void getRcPaint(RECT *r);

private:	// NONPORTABLE
  OSWINDOWHANDLE hWnd;
#ifdef WIN32
  PAINTSTRUCT ps;
#endif
};

class BltCanvas;
class PaintBltCanvas : public Canvas {
public:
  PaintBltCanvas();
  virtual ~PaintBltCanvas();
  int beginPaint(BaseWnd *basewnd);
  int beginPaintNC(BaseWnd *basewnd);

  void *getBits();
  void getRcPaint(RECT *r);

private:	// NONPORTABLE
  OSWINDOWHANDLE hWnd;
#ifdef WIN32
  PAINTSTRUCT ps;
#endif
  HDC wnddc;
  HBITMAP hbmp;
#ifdef WIN32
  HBITMAP prevbmp;
#endif
  BOOL nonclient;
#ifdef LINUX
  BltCanvas *blitter;
#endif
};

class Bitmap;

class MemCanvas : public Canvas {
public:
  MemCanvas(void *bits=NULL, int w=0, int h=0, int p=0);
  MemCanvas(Bitmap *bmp);
  virtual ~MemCanvas();

  int createCompatible(Canvas *canvas);
  int createCompatible(HDC dc);

  void fillBits(COLORREF color, RECT *r=NULL);
private:
};

class DCCanvas : public Canvas {
public:
  DCCanvas(HDC clone=NULL, BaseWnd *srcWnd=NULL);
  virtual ~DCCanvas();

  int cloneDC(HDC clone, BaseWnd *srcWnd=NULL);
};

// this is a canvas that represents the whole display
class SysCanvas : public Canvas {
public:
  SysCanvas();
  virtual ~SysCanvas();
};

class DCBltCanvas : public Canvas {
public:
  DCBltCanvas();
  virtual ~DCBltCanvas();

  int cloneDC(HDC clone, RECT *r, BaseWnd *srcWnd=NULL);
  int setOrigDC(HDC neworigdc); // set to null to prevent commitdc on delete, non null to change destination dc
  int commitDC(void);						// allows commit to DC without deleting
#if 0
  int cloneCanvas(CanvasBase *clone, RECT *r);
#endif

private:
  HDC origdc;
  RECT rect;
  HBITMAP hbmp;
#ifdef WIN32
  HBITMAP prevbmp;
#endif
#ifdef LINUX
  BltCanvas *blitter;
#endif
};

// note: getBaseWnd() returns NULL for this class
class BaseCloneCanvas : public Canvas {
public:
  BaseCloneCanvas(CanvasBase *cloner=NULL);
  virtual ~BaseCloneCanvas();

  int clone(CanvasBase *cloner);
};

#ifdef WIN32
class DDSurfaceCanvas : public Canvas {
public:
  DDSurfaceCanvas(LPDIRECTDRAWSURFACE surface, int w, int h);
  virtual ~DDSurfaceCanvas();

  int isready();
  void enter();
  void exit();

private:
  LPDIRECTDRAWSURFACE surf;
  int _w, _h;
};
#endif

enum
{
  ALIGN_LEFT,
  ALIGN_CENTER,
  ALIGN_RIGHT
};

#endif
