#ifndef _SVC_TEXTFEED_H
#define _SVC_TEXTFEED_H

#include <bfc/dispatch.h>
#include <api/service/services.h>
#include <bfc/string/string.h>

class Dependent;

namespace WaSvc {
  const GUID TEXTFEED=SVCTGUID_MK4CC('t','x','t','f');
};

class NOVTABLE svc_textFeed : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::TEXTFEED; }
  static const GUID *depend_getClassGuid() {
    // {A04E3420-CBA1-4ae1-B3C0-8DE127D2B861}
    static const GUID ret = 
    { 0xa04e3420, 0xcba1, 0x4ae1, { 0xb3, 0xc0, 0x8d, 0xe1, 0x27, 0xd2, 0xb8, 0x61 } };
    return &ret;
  }
  static const char *getServiceName() { return "Untitled Textfeed"; }
  
  int hasFeed(const char *name);
  const char *getFeedText(const char *name);
  const char *getFeedDescription(const char *name);

  Dependent *getDependencyPtr();

  enum {
    Event_TEXTCHANGE=100, // param is const char* to id, ptr points to new text
  };

  enum {
    SVCTEXTFEED_HASFEED=10,
//20,30 retired
    SVCTEXTFEED_GETFEEDTEXT=40,
    SVCTEXTFEED_GETFEEDDESC=45,
    SVCTEXTFEED_GETDEPENDENCYPTR=100,
  };

};

inline int svc_textFeed::hasFeed(const char *name) {
  return _call(SVCTEXTFEED_HASFEED, 0, name);
}

inline const char *svc_textFeed::getFeedText(const char *name) {
  return _call(SVCTEXTFEED_GETFEEDTEXT, (const char *)NULL, name);
}

inline const char *svc_textFeed::getFeedDescription(const char *name) {
  return _call(SVCTEXTFEED_GETFEEDDESC, (const char *)NULL, name);
}

inline Dependent *svc_textFeed::getDependencyPtr() {
  return _call(SVCTEXTFEED_GETDEPENDENCYPTR, (Dependent*)NULL);
}

class NOVTABLE svc_textFeedI : public svc_textFeed {
public:
  virtual int hasFeed(const char *name)=0;
  virtual const char *getFeedText(const char *name)=0;
  virtual const char *getFeedDescription(const char *name)=0;
  virtual Dependent *getDependencyPtr()=0;
  virtual void *dependent_getInterface(const GUID *classguid);//implemented for you

protected:
  RECVS_DISPATCH;
};

// see helper class TextFeed

#include <api/service/servicei.h>
template <class T>
class TextFeedCreatorSingle : public waServiceFactoryTSingle<svc_textFeed, T> {
public:
  svc_textFeed *getFeed() {
    return getSingleService();
  }
};

#include <api/service/svc_enum.h>

class TextFeedEnum : public SvcEnumT<svc_textFeed> {
public:
  TextFeedEnum(const char *_feedid) : feedid(_feedid) {}
protected:
  virtual int testService(svc_textFeed *svc) {
    return (svc->hasFeed(feedid));
  }
private:
  String feedid;
};

#endif
