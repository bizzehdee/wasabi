#ifndef _SVC_XUIOBJECT_H
#define _SVC_XUIOBJECT_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

#ifdef LEGACY_XUI
#define WIDGET_CLASS GuiObject
#else
#define WIDGET_CLASS XUIObject
#endif

class WIDGET_CLASS;
class XmlReaderParams;

class svc_xuiObject : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::XUIOBJECT; }
  int testTag(const char *xmltag);
  WIDGET_CLASS *instantiate(const char *xmltag, XmlReaderParams *params=NULL);
  void destroy(WIDGET_CLASS *g);

  enum {
    XUI_TESTTAG=10,
    //XUI_INSTANTIATE=20, // RETIRED
    XUI_INSTANTIATEWITHPARAMS=25,
    XUI_DESTROY=30,
  };
};

inline int svc_xuiObject::testTag(const char *xmltag) {
  return _call(XUI_TESTTAG, 0, xmltag);
}

inline WIDGET_CLASS *svc_xuiObject::instantiate(const char *xmltag, XmlReaderParams *params) {
  return _call(XUI_INSTANTIATEWITHPARAMS, (WIDGET_CLASS *)NULL, xmltag, params);
}

inline void svc_xuiObject::destroy(WIDGET_CLASS *o) {
  _voidcall(XUI_DESTROY, o);
}

// derive from this one
class svc_xuiObjectI : public svc_xuiObject {
public:
  virtual int testTag(const char *xmltag)=0;
  virtual WIDGET_CLASS *instantiate(const char *xmltag, XmlReaderParams *params=NULL)=0;
  virtual void destroy(WIDGET_CLASS *o)=0;

protected:
  RECVS_DISPATCH;
};

#ifndef LEGACY_XUI
#include <api/service/servicei.h>
template <class T>
class XuiObjectCreator : public waServiceFactoryTSingle<svc_xuiObject, T> {};

template <class T>
class XuiSingleInstantiatorSvc : public svc_xuiObjectI {
  public:
#ifdef LEGACY_XUI
    static const char *xuisvc_getXmlTag() { return T::xuiobject_getXmlTag(); }
#endif
    int testTag(const char *xmltag) {
      if (STRCASEEQL(xmltag, T::getXmlTag())) return 1;
      return 0;
    }
    XUIObject *instantiate(const char *xmltag, XmlReaderParams *params=NULL) {
      if (testTag(xmltag)) {
        T *obj = new T;
        ASSERT(obj != NULL);
        XUIObject *xo = obj->getXUIObject();
        ASSERT(xo != NULL);
        xo->setOwnerService(this);
        return xo;
      }
      return NULL;
    }
  void destroy(XUIObject *g) {
    T *obj = static_cast<T *>(g->getRootWnd());
    delete obj;
  }
  static const char *getServiceName() { return StringPrintf("Xui Instantiator Service for %s", T::getXmlTag()); }
};

#ifdef _X86_
#define _DECLARE_XUISERVICE(XUICLASS) \
BEGIN_SERVICES(XUICLASS##Svc) \
DECLARE_SERVICE(XuiObjectCreator< XuiSingleInstantiatorSvc< XUICLASS > >); \
END_SERVICES(XUICLASS##Svc, _##XUICLASS##Svc); \
extern "C" { int _link_##XUICLASS; }
#else
#define _DECLARE_XUISERVICE(XUICLASS) \
BEGIN_SERVICES(XUICLASS##Svc) \
DECLARE_SERVICE(XuiObjectCreator< XuiSingleInstantiatorSvc< XUICLASS > >); \
END_SERVICES(XUICLASS##Svc, _##XUICLASS##Svc); \
extern "C" { int __link_##XUICLASS; }
#endif

#define MAKE_XUISERVICE_CLASS(CLASS, WNDCLASS, XUICLASS, TAGNAME) \
class CLASS: public XUIWnd<WNDCLASS, XUICLASS> { \
  public: \
    static const char *getXmlTag() { return TAGNAME; } \
};

#else
#include <api/service/servicei.h>
template <class T>
class XuiObjectCreator : public waServiceFactoryTSingle<svc_xuiObject, T> {
public:
  virtual const char *svc_getTestString() { return T::xuisvc_getXmlTag(); }
};
#endif


#include <api/service/svc_enum.h>
#include <bfc/string/string.h>

class XuiObjectSvcEnum : public SvcEnumT<svc_xuiObject> {
public:
  XuiObjectSvcEnum(const char *xmltag) : tag(xmltag) {}
protected:
  virtual int testService(svc_xuiObject *svc) {
    return (svc->testTag(tag));
  }
private:
  String tag;
};

#endif
