#ifndef _SVC_SKINFILTER_H
#define _SVC_SKINFILTER_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

class NOVTABLE svc_skinFilter : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::SKINFILTER; }
  int filterBitmap(unsigned char *bits, int w, int h, int bpp, const char *element_id, const char *forcegroup=NULL);
  ARGB32 filterColor(ARGB32 color, const char *element_id, const char *forcegroup=NULL);
                                                                                      
  enum {
    FILTERBITMAP=100,
    FILTERCOLOR=200,
  };
};

inline int svc_skinFilter::filterBitmap(unsigned char *bits, int w, int h, int bpp, const char *element_id, const char *forcegroup) {
  return _call(FILTERBITMAP, 0, bits, w, h, bpp, element_id, forcegroup);
}

inline ARGB32 svc_skinFilter::filterColor(ARGB32 color, const char *element_id, const char *forcegroup) {
  return _call(FILTERCOLOR, (ARGB32)0, color, element_id, forcegroup);
}

// derive from this one
class NOVTABLE svc_skinFilterI : public svc_skinFilter {
public:
  virtual int filterBitmap(unsigned char *bits, int w, int h, int bpp, const char *element_id, const char *forcegroup=NULL)=0;
  virtual ARGB32 filterColor(ARGB32 color, const char *element_id, const char *forcegroup=NULL) {
    // easy cheesy helper
    filterBitmap((unsigned char *)&color, 1, 1, 32, element_id, forcegroup);
    return color;
  }

protected:
  RECVS_DISPATCH;
};

#include <api/service/svc_enum.h>

class SkinFilterEnum : public SvcEnumT<svc_skinFilter> {
public:
  SkinFilterEnum() {}
protected:
  virtual int testService(svc_skinFilter* svc) {
    return TRUE;
  }
};

#endif
