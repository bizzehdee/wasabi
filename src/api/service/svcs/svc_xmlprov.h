#ifndef _SVC_XMLPROVIDER_H
#define _SVC_XMLPROVIDER_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

class XmlReaderParams;

class NOVTABLE svc_xmlProvider : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::XMLPROVIDER; }

  int testDesc(const char *desc);
  const char *getXmlData(const char *desc, const char *incpath, XmlReaderParams *params=NULL);

  enum {
    TESTDESC=10,
    GETXMLDATA=20,
  };
};

inline int svc_xmlProvider::testDesc(const char *desc) {
  return _call(TESTDESC, 0, desc);
}

inline const char *svc_xmlProvider::getXmlData(const char *desc, const char *incpath, XmlReaderParams *params) {
  return _call(GETXMLDATA, (const char *)0, desc, incpath, params);
}

// derive from this one
class NOVTABLE svc_xmlProviderI : public svc_xmlProvider {
public:
  virtual int testDesc(const char *desc)=0;
  virtual const char *getXmlData(const char *desc, const char *incpath, XmlReaderParams *params=NULL)=0;

protected:
  RECVS_DISPATCH;
};

#include <api/service/svc_enum.h>
#include <bfc/string/string.h>

class XmlProviderEnum : public SvcEnumT<svc_xmlProvider> {
public:
  XmlProviderEnum(const char *_desc) : desc(_desc) { }

protected:
  virtual int testService(svc_xmlProvider *svc) {
    return svc->testDesc(desc);
  }

private:
  String desc;
};

#endif
