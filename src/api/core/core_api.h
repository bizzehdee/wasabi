#ifndef __WASABI_CORE_API_H_
#define __WASABI_CORE_API_H_

#include <bfc/std.h>
#include <bfc/thread.h>
#include <bfc/string/string.h>
#include <api/service/services.h>

#include <parse/pathparse.h>
#include <api/core/buttons.h>
#include <api/core/coreservices.h>
#include <api/core/corehandle.h>
#include <api/core/chunklist.h>
#include <api/core/corecb.h>
#include <api/core/mediainfo.h>

#include <api/service/services.h>
#include <api/service/svcs/svc_mediacore.h>
#include <api/service/svcs/svc_coreadmin.h>
#include <api/service/svcs/svc_mediaconverter.h>

#include "api_core.h"

class CallbackMultiplex : public CoreCallback {
public:
  CallbackMultiplex(svc_mediaCoreI *_core);

  void add(CoreCallback *c);
  void del(CoreCallback *c);
  virtual void setSequencer(ItemSequencer *_seq);
  virtual void delSequencer(ItemSequencer *_seq);
  virtual ItemSequencer *getSequencer();
  virtual const char *getCurrent();
  virtual void setCurrent(String _current);
  virtual int _dispatch(int msg, void *retval, void **params=NULL, int nparam=0);
  virtual void notify_seq(int msg);

private:
  svc_mediaCoreI *core;
  PtrList<CoreCallback> list;
  ItemSequencer *seq;
  String Current;
};

class Core: public svc_mediaCore {
public:
  Core(svc_mediaCoreI *_core, CoreToken tok, const char *_name);
  ~Core();

  virtual CoreToken getToken();
  virtual const char * getServiceName();
  virtual int _dispatch(int msg, void *retval, void **params=NULL, int nparam=0);
  virtual void setSequencer(ItemSequencer *_seq);
  virtual void delSequencer(ItemSequencer *_seq);
  virtual ItemSequencer *getSequencer();
  virtual void addCallback(CoreCallback *cb);
  virtual void delCallback(CoreCallback *cb);
  virtual const char *getCurrent() ;
  virtual void setCurrent(String _current);
private:
  svc_mediaCoreI* core;
  CoreToken m_coretoken;
  String name;
  CallbackMultiplex cmul;
};

typedef struct {
  String name;
  String ext;
  String family;
} SCAextension;

class core_api : public api_coreI {
public:
  core_api() {
    initDaMainCore();
  }

  ~core_api() {

  }

  static const char *getServiceName() { return "PlaybackCore API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

  virtual const char *core_getSupportedExtensions();
  virtual const char *core_getExtSupportedExtensions();
  virtual CoreToken core_create();
  virtual int core_free(CoreToken core);
  virtual int core_setNextFile(CoreToken core, const char *playstring);
  virtual int core_getStatus(CoreToken core);
  virtual const char *core_getCurrent(CoreToken core);
  virtual int core_getCurPlaybackNumber(CoreToken core);
  virtual int core_getPosition(CoreToken core);
  virtual int core_getWritePosition(CoreToken core);
  virtual int core_setPosition(CoreToken core, int ms);
  virtual int core_getLength(CoreToken core);
  virtual int core_getPluginData(const char *playstring, const char *name, char *data, int data_len, int data_type = 0);
  virtual unsigned int core_getVolume(CoreToken core);
  virtual void core_setVolume(CoreToken core, unsigned int vol);
  virtual int core_getPan(CoreToken core);
  virtual void core_setPan(CoreToken core, int val);
  virtual void core_addCallback(CoreToken core, CoreCallback *cb);
  virtual void core_delCallback(CoreToken core, CoreCallback *cb);
  virtual int core_getVisData(CoreToken core, void *dataptr, int sizedataptr);
  virtual int core_getLeftVuMeter(CoreToken core);
  virtual int core_getRightVuMeter(CoreToken core);
  virtual int core_registerSequencer(CoreToken core, ItemSequencer *seq);
  virtual int core_deregisterSequencer(CoreToken core, ItemSequencer *seq);
  virtual void core_userButton(CoreToken core, int button);
  virtual int core_getEqStatus(CoreToken core);
  virtual void core_setEqStatus(CoreToken core, int enable);
  virtual int core_getEqPreamp(CoreToken core);
  virtual void core_setEqPreamp(CoreToken core, int pre);
  virtual int core_getEqBand(CoreToken core, int band);
  virtual void core_setEqBand(CoreToken core, int band, int val);
  virtual int core_getEqAuto(CoreToken core);
  virtual void core_setEqAuto(CoreToken core, int enable);
  virtual void core_setCustomMsg(CoreToken core, const char *text);
  virtual void core_registerExtension(const char *extensions, const char *extension_name, const char *family = NULL);
  virtual const char *core_getExtensionFamily(const char *extension);
  virtual void core_unregisterExtension(const char *extensions);
  virtual const char *core_getTitle();

private:
  void initDaMainCore();
  void addfamily(const char *name);
  Core * tokenToCore(CoreToken tok);
  PtrList<Core> cores;
  PtrList<SCAextension> SCAextensions;
  PtrList<String> SCAfamilies;
};


#endif //!__WASABI_CORE_API_H_
