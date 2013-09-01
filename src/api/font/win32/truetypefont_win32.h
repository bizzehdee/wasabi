#ifndef __TRUETYPEFONT_WN32_H
#define __TRUETYPEFONT_WN32_H

#include <api/service/svcs/svc_font.h>
#include <bfc/string/string.h>
#include <bfc/stack.h>

#include <api/font/truetypefontdef.h>

class CanvasBase;
class BltCanvas;

class TrueTypeFont_Win32 : public svc_fontI {
 public:
  TrueTypeFont_Win32();
  virtual ~TrueTypeFont_Win32();

  virtual void textOut(CanvasBase *c, int x, int y, const char *txt, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  virtual void textOut2(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  virtual void textOutEllipsed(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  virtual void textOutWrapped(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  virtual void textOutWrappedPathed(CanvasBase *c, int x, int y, int w, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  virtual void textOutCentered(CanvasBase *c, RECT *r, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  virtual int getTextWidth(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias);
  virtual int getTextHeight(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias);
  virtual int getTextHeight2(CanvasBase *c, int size, int bold, int underline, int italic, int antialias);
  virtual void getTextExtent(CanvasBase *c, const char *text, int *w, int *h, int size, int bold, int underline, int italic, int antialias);

  virtual void setFontId(const char *id) { font_id = id; }
  virtual const char *getFontId() { return font_id; }
  virtual int getScriptId() { return scriptid; }
  virtual void setScriptId(int id) { scriptid = id; }

  virtual int isBitmap();
  virtual void setFontFace(const char *face);
  virtual int addFontResource(FILE *f, const char *name);
  virtual int addFontResource2(void *mem, int datalen, const char *name) { return 0; }//FUCKO: dunno what to do here?
  virtual const char *getFaceName();

  virtual const char *getFontSvcName() { return "Win32 TextOut"; }
  static const char *getServiceName() { return "Win32 Truetype Font Service"; }

  static char *filenameToFontFace(const char *filename);

 protected:
  void prepareCanvas(CanvasBase *c, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor);
  void restoreCanvas(CanvasBase *c);

 protected:
  String font_id;
  int scriptid;

 private:
  CanvasBase *prepareAntialias(CanvasBase *c, int x, int y, const char *txt, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int w, int h);
  void completeAntialias(CanvasBase *c);

  String face_name;
  String tmpfilename;
  int DColdstate;
  HFONT font;
  Stack<fontslot*> fontstack;
  BltCanvas *antialias_canvas;
  int al_w, al_h, al_x, al_y, al_xo, al_yo, al_dw, al_dh;
  COLORREF al_mask;
};

#endif
