#ifndef _SVC_IMGGEN_H
#define _SVC_IMGGEN_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

#ifdef WASABI_API_XML
class XmlReaderParams;
#else
#define XmlReaderParams void
#endif

class NOVTABLE svc_imageGenerator : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::IMAGEGENERATOR; }
  int testDesc(const char *desc);
  ARGB32 *genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params=NULL);
  int outputCacheable();

  enum {
    TESTDESC=10,
    GENIMAGE=30,
    OUTPUTCACHEABLE=40,
  };
};

inline int svc_imageGenerator::testDesc(const char *desc) {
  return _call(TESTDESC, 0, desc);
}

inline ARGB32 *svc_imageGenerator::genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params) {
  return _call(GENIMAGE, (ARGB32 *)0, desc, has_alpha, w, h, params);
}

inline int svc_imageGenerator::outputCacheable() {
  return _call(OUTPUTCACHEABLE, 0);
}

// derive from this one
class NOVTABLE svc_imageGeneratorI : public svc_imageGenerator {
public:
  virtual int testDesc(const char *desc)=0;
  virtual ARGB32 *genImage(const char *desc, int *has_alpha, int *w, int *h, XmlReaderParams *params=NULL)=0;
  virtual int outputCacheable() { return 0; }

protected:
  RECVS_DISPATCH;
};

#include <api/service/svc_enum.h>
#include <bfc/string/string.h>

class ImgGeneratorEnum : public SvcEnumT<svc_imageGenerator> {
public:
  ImgGeneratorEnum(const char *_desc) : desc(_desc) { }

protected:
  virtual int testService(svc_imageGenerator *svc) {
    return svc->testDesc(desc);
  }

private:
  String desc;
};

#endif
