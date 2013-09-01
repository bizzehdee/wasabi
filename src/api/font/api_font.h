#ifndef __API_FONT_H
#define __API_FONT_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

class CanvasBase;

class api_font : public Dispatchable {
public:
  void font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt);
  int font_getInfo(CanvasBase *c, const char *font, int infoid, const char *txt, int *w, int *h);	// BU note: this no longer returns the width or height in the retval, only in the *w and *h. retval is now 1 for success, 0 for failure

  enum {
    API_FONT_FONT_TEXTOUT = 0,
    API_FONT_FONT_GETINFO = 10,
  };
};

inline void api_font::font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt) {
  _voidcall(API_FONT_FONT_TEXTOUT, c, style, x, y, w, h, txt);
}

inline int api_font::font_getInfo(CanvasBase *c, const char *font, int infoid, const char *txt, int *w, int *h) {
  return _call(API_FONT_FONT_GETINFO, (int)0, c, font, infoid, txt, w, h);
}

class api_fontI : public api_font {
public:
  static const char *getServiceName() { return "Font Rendering API"; }
  static GUID getServiceType() { return WaSvc::FONTRENDER; }
  
  virtual void font_textOut(CanvasBase *c, int style, int x, int y, int w, int h, const char *txt)=0;
  virtual int font_getInfo(CanvasBase *c, const char *font, int infoid, const char *txt, int *w, int *h)=0;

protected:
  api_fontI() {}
  virtual ~api_fontI() {}

  RECVS_DISPATCH;
};

// {1FCA9C7E-5923-4b9c-8906-0F8C331DF21C}
static const GUID fontApiServiceGuid = 
{ 0x1fca9c7e, 0x5923, 0x4b9c, { 0x89, 0x6, 0xf, 0x8c, 0x33, 0x1d, 0xf2, 0x1c } };

extern api_font *fontApi;

#endif
