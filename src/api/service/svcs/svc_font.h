#ifndef _SVC_FONT_H
#define _SVC_FONT_H

#include <bfc/dispatch.h>
#include <bfc/string/string.h>
#include <api/service/svc_enum.h>
#include <api/service/services.h>
#include <api/service/servicei.h>

class CanvasBase;

class NOVTABLE svc_font : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::FONTRENDER; }

  void textOut(CanvasBase *c, int x, int y, const char *txt, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);                           // abstract interface
  void textOut(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  void textOutEllipsed(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  void textOutWrapped(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  void textOutWrappedPathed(CanvasBase *c, int x, int y, int w, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  void textOutCentered(CanvasBase *c, RECT *r, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias);
  int getTextWidth(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias);
  int getTextHeight(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias);
  int getTextHeight(CanvasBase *c, int size, int bold, int underline, int italic, int antialias);
  void getTextExtent(CanvasBase *c, const char *text, int *w, int *h, int size, int bold, int underline, int italic, int antialias);

  void setFontId(const char *id);
  const char *getFontId();
  const char *getFaceName();
  int isBitmap();
  int getScriptId();
  void setScriptId(int id);

  void setFontFace(const char *face);
  int addFontResource(FILE *f, const char *name);
  int addFontResource2(void *mem, int datalen, const char *name);

  const char *getFontSvcName();

protected:
  enum {
    TEXTOUT,
    TEXTOUT2,
    TEXTOUTELLIPSED,
    TEXTOUTWRAPPED,
    TEXTOUTWRAPPEDPATHED,
    TEXTOUTCENTERED,
    GETTEXTWIDTH,
    GETTEXTHEIGHT,
    GETTEXTHEIGHT2,
    GETTEXTEXTENT,
    SETFONTID,
    GETFONTID,
    GETFACENAME_,	// GETFACENAME is taken in win32
    ISBITMAP,
    GETSCRIPTID,
    SETSCRIPTID,
    SETFONTFACE,
    ADDFONTRESOURCE,
    ADDFONTRESOURCE2,
    GETFONTSVCNAME,
  };
};

inline void svc_font::textOut(CanvasBase *c, int x, int y, const char *txt, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias) {
  _voidcall(TEXTOUT, c, x, y, txt, size, bold, opaque, underline, italic, color, bkcolor, xoffset, yoffset, antialias);
}

inline void svc_font::textOut(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias) {
  _voidcall(TEXTOUT2, c, x, y, w, h, txt, size, bold, opaque, underline, italic, align, color, bkcolor, xoffset, yoffset, antialias);
}

inline void svc_font::textOutEllipsed(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias) {
  _voidcall(TEXTOUTELLIPSED, c, x, y, w, h, txt, size, bold, opaque, underline, italic, align, color, bkcolor, xoffset, yoffset, antialias);
}

inline void svc_font::textOutWrapped(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias) {
  _voidcall(TEXTOUTWRAPPED, c, x, y, w, h, txt, size, bold, opaque, underline, italic, align, color, bkcolor, xoffset, yoffset, antialias);
}

inline void svc_font::textOutWrappedPathed(CanvasBase *c, int x, int y, int w, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias) {
  _voidcall(TEXTOUTWRAPPEDPATHED, c, x, y, w, txt, size, bold, opaque, underline, italic, align, color, bkcolor, xoffset, yoffset, antialias);
}

inline void svc_font::textOutCentered(CanvasBase *c, RECT *r, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias) {
  _voidcall(TEXTOUTCENTERED, c, r, txt, size, bold, opaque, underline, italic, align, color, bkcolor, xoffset, yoffset, antialias);
}

inline int svc_font::getTextWidth(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias) {
  return _call(GETTEXTWIDTH, (int)0, c, text, size, bold, underline, italic, antialias);
}

inline int svc_font::getTextHeight(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias) {
  return _call(GETTEXTHEIGHT, (int)0, c, text, size, bold, underline, italic, antialias);
}

inline int svc_font::getTextHeight(CanvasBase *c, int size, int bold, int underline, int italic, int antialias) {
  return _call(GETTEXTHEIGHT, (int)0, c, size, bold, underline, italic, antialias);
}

inline void svc_font::getTextExtent(CanvasBase *c, const char *text, int *w, int *h, int size, int bold, int underline, int italic, int antialias) {
  _voidcall(GETTEXTEXTENT, c, text, w, h, size, bold, underline, italic, antialias);
}

inline void svc_font::setFontId(const char *id) {
  _voidcall(SETFONTID, id);
}

inline const char *svc_font::getFontId() {
  return _call(GETFONTID, (const char *)0);
}

inline const char *svc_font::getFaceName() {
  return _call(GETFACENAME_, (const char *)0);
}

inline int svc_font::isBitmap() {
  return _call(ISBITMAP, (int)0);
}

inline int svc_font::getScriptId() {
  return _call(GETSCRIPTID, (int)0);
}

inline void svc_font::setScriptId(int id) {
  _voidcall(SETSCRIPTID, id);
}

inline void svc_font::setFontFace(const char *face) {
  _voidcall(SETFONTFACE, face);
}

inline int svc_font::addFontResource(FILE *f, const char *name) {
  return _call(ADDFONTRESOURCE, (int)0, f, name);
}

inline int svc_font::addFontResource2(void *mem, int datalen, const char *name) {
  return _call(ADDFONTRESOURCE2, (int)0, mem, datalen, name);
}

inline const char *svc_font::getFontSvcName() {
  return _call(GETFONTSVCNAME, (const char *)0);
}


// implementor derives from this one
class NOVTABLE svc_fontI : public svc_font {
public:

  virtual void textOut(CanvasBase *c, int x, int y, const char *txt, int size, int bold, int opaque, int underline, int italic, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias)=0;                           // abstract interface
  virtual void textOut2(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias)=0;
  virtual void textOutEllipsed(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias)=0;
  virtual void textOutWrapped(CanvasBase *c, int x, int y, int w, int h, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias)=0;
  virtual void textOutWrappedPathed(CanvasBase *c, int x, int y, int w, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias)=0;
  virtual void textOutCentered(CanvasBase *c, RECT *r, const char *txt, int size, int bold, int opaque, int underline, int italic, int align, COLORREF color, COLORREF bkcolor, int xoffset, int yoffset, int antialias)=0;
  virtual int getTextWidth(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias)=0;
  virtual int getTextHeight(CanvasBase *c, const char *text, int size, int bold, int underline, int italic, int antialias)=0;
  virtual int getTextHeight2(CanvasBase *c, int size, int bold, int underline, int italic, int antialias)=0;
  virtual void getTextExtent(CanvasBase *c, const char *text, int *w, int *h, int size, int bold, int underline, int italic, int antialias)=0;

  virtual void setFontId(const char *id)=0;
  virtual const char *getFontId()=0;
  virtual const char *getFaceName()=0;
  virtual int isBitmap()=0;
  virtual int getScriptId()=0;
  virtual void setScriptId(int id)=0;

  virtual void setFontFace(const char *face)=0;
  virtual int addFontResource(FILE *f, const char *name)=0;
  virtual int addFontResource2(void *mem, int datalen, const char *name)=0;

  virtual const char * getFontSvcName()=0;

protected:
  RECVS_DISPATCH;
};

class FontSvcEnum : public SvcEnumT<svc_font> {
public:
  FontSvcEnum(const char *_svc_name = NULL) : svc_name(_svc_name) {}
protected:
  virtual int testService(svc_font *svc) {
    if (!svc_name.len()) return 1; // blank name returns all services.
    return (STRCASEEQL(svc->getFontSvcName(),svc_name));
  }
private:
  String svc_name;
};

template <class T>
class FontCreator : public waServiceFactoryT<svc_font, T> {};



#endif // _SVC_FONT_H
