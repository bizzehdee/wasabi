#include <precomp.h>
#include <wasabicfg.h>
#include <bfc/assert.h>
#include <wac/wac.h>

#include <api/service/servicei.h>

DECLARE_MODULE_SVCMGR

WAComponentClient::WAComponentClient(const char *name) : OSModuleHandle(NULL)
#ifdef WASABI_API_CONFIG
  , CfgItemI(name) 
#endif
{
}

const char *WAComponentClient::getName() {
#ifdef WASABI_API_CONFIG
  return cfgitem_getName();
#endif
  return NULL;
}

#ifdef WASABINOMAINAPI
void WAComponentClient::registerServices(api_service *_serviceapi) {
  serviceApi = _serviceapi;

  waServiceFactory *sf = serviceApi->service_getServiceByGuid(applicationApiServiceGuid);
  if (sf) applicationApi = reinterpret_cast<api_application*>(sf->getInterface());

#ifdef WASABI_COMPILE_SYSCB
  sf = serviceApi->service_getServiceByGuid(syscbApiServiceGuid);
  if (sf) sysCallbackApi = reinterpret_cast<api_syscb *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_MEMMGR
  sf = serviceApi->service_getServiceByGuid(memmgrApiServiceGuid);
  if (sf) memmgrApi = reinterpret_cast<api_memmgr *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_CONFIG
  sf = serviceApi->service_getServiceByGuid(configApiServiceGuid);
  if (sf) configApi = reinterpret_cast<api_config *>(sf->getInterface());
#endif

  // if no static service was ever declared, we need to init it so we don't hit null
  INIT_MODULE_SVCMGR();

  DebugString("Auto-initializing %d services (system pass)\n", staticServiceMgr->__m_modules2.getNumItems());
  MODULE_SVCMGR_ONINIT2();

#ifdef WASABI_COMPILE_IMGLDR
  sf = serviceApi->service_getServiceByGuid(imgLdrApiServiceGuid);
  if (sf) imgLoaderApi = reinterpret_cast<imgldr_api  *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_FONTS
  sf = serviceApi->service_getServiceByGuid(fontApiServiceGuid);
  if (sf) fontApi = reinterpret_cast<api_font *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_XMLPARSER
  sf = serviceApi->service_getServiceByGuid(xmlApiServiceGuid);
  if (sf) xmlApi = reinterpret_cast<api_xml *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_FILEREADER
  sf = serviceApi->service_getServiceByGuid(fileApiServiceGuid);
  if (sf) fileApi = reinterpret_cast<api_filereader *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_LOCALES
  sf = serviceApi->service_getServiceByGuid(localesGuid);
  if (sf) localeApi = reinterpret_cast<api_locales *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_TIMERS
  sf = serviceApi->service_getServiceByGuid(timerApiServiceGuid);
  if (sf) timerApi = reinterpret_cast<timer_api *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_WND
  sf = serviceApi->service_getServiceByGuid(wndApiServiceGuid);
  if (sf) wndApi = reinterpret_cast<wnd_api *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_WNDMGR
  sf = serviceApi->service_getServiceByGuid(wndMgrApiServiceGuid);
  if (sf) wndManagerApi = reinterpret_cast<wndmgr_api *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_COMPONENTS
#endif

#ifdef WASABI_COMPILE_METADB
#endif

#ifdef WASABI_COMPILE_SCRIPT
  sf = serviceApi->service_getServiceByGuid(makiApiServiceGuid);
  if (sf) makiApi = reinterpret_cast<api_maki *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_SKIN
  sf = serviceApi->service_getServiceByGuid(skinApiServiceGuid);
  if (sf) skinApi = reinterpret_cast<api_skin *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_MAKIDEBUG
  sf = serviceApi->service_getServiceByGuid(makiDebugApiServiceGuid);
  if (sf) debugApi = reinterpret_cast<api_makiDebugger *>(sf->getInterface());
#endif

#ifdef WASABI_COMPILE_MEDIACORE
  sf = serviceApi->service_getServiceByGuid(coreApiServiceGuid);
  if (sf) coreApi = reinterpret_cast<api_core *>(sf->getInterface());
#endif

#else
void WAComponentClient::registerServices(ComponentAPI *_api) {
  // save API *
  api = _api;
#endif

#ifdef WASABI_API_CONFIG
  // propagate name from component to cfgitemi
  if (cfgitem_getName() == NULL) CfgItemI::setName(WaComponent::getName());
#endif

  DebugString("Auto-initializing %d services (user pass)\n", staticServiceMgr->__m_modules.getNumItems());
  MODULE_SVCMGR_ONINIT();

  // register resources
  foreach(resources)
    resources.getfor()->onRegisterServices();
  endfor

#ifdef WASABI_API_CONFIG

  // our CfgItem GUID defaults to our component GUID
  cfgitem_setGUID(getGUID());

  // by default, peg us under "Installed Components" in config
  // {99CFD75C-1CA7-49e5-B8C0-7D78AA443C10}
  const GUID installed_guid = 
  { 0x99cfd75c, 0x1ca7, 0x49e5, { 0xb8, 0xc0, 0x7d, 0x78, 0xaa, 0x44, 0x3c, 0x10 } };
  if (cfgitem_getParentGuid() == INVALID_GUID)
    setParentGuid(installed_guid);

  WASABI_API_CONFIG->config_registerCfgItem(this);
#endif

  // allow overridden component class to do stuff
  onRegisterServices();
}

void WAComponentClient::deregisterServices() {
  MODULE_SVCMGR_ONSHUTDOWN();
  foreach(resources)
    resources.getfor()->onDeregisterServices();
  endfor
  MODULE_SVCMGR_ONSHUTDOWN2();
#ifdef WASABI_API_CONFIG
  WASABI_API_CONFIG->config_deregisterCfgItem(this);
#endif
}


CfgItem *WAComponentClient::getCfgInterface(int n) {
#ifdef WASABI_API_CONFIG
  if (n == 0) return this;
#endif
  return NULL;
}


class RegService : public LoadableResource {
public:
  RegService(waServiceFactoryI *sf) : factory(sf) {}

  virtual void onRegisterServices() {
    WASABI_API_SVC->service_register(factory);
  }
  virtual void onDeregisterServices() {
    WASABI_API_SVC->service_deregister(factory);
  }

private:
  waServiceFactoryI *factory;
};

void WAComponentClient::registerService(waServiceFactoryI* service) {
  registerResource(new RegService(service));
}



class SkinPart : public LoadableResource {
public:
  SkinPart(const char *name, int rel) : filename(name), id(-1), relative(rel) {}

  void beforeLoadingSkinElements() {
#ifdef WASABI_COMPILE_SKIN
    ASSERT(!filename.isempty());
    String fn(filename);
    switch (relative) {
      case RSF_RELATIVETOWAC:
        fn.prepend(the->getComponentPath());// api->path_getComponentPath());
      break;
      case RSF_RELATIVETOTHEME: {
        char theme[WA_MAX_PATH]="";
        WASABI_API_CONFIG->getStringPrivate("theme", theme, WA_MAX_PATH, "default");
        fn.prepend(StringPrintf("%s%sthemes%s%s%s", WASABI_API_APP->path_getAppPath(), DIRCHARSTR, DIRCHARSTR, theme, DIRCHARSTR).getValue());
      }
      break;
    }
    id = WASABI_API_SKIN->loadSkinFile(fn);
#endif
  }
  virtual void onDeregisterServices() {
#ifdef WASABI_COMPILE_SKIN
    if (id >= 0 && WASABI_API_SKIN) WASABI_API_SKIN->unloadSkinPart(id);
    id = -1;
#endif
  }

private:
  String filename;
  int id;
  int relative;
};

void WAComponentClient::registerSkinFile(const char *filename, int relative) {
  registerResource(new SkinPart(filename, relative));
}

class AutoPop : public LoadableResource {
public:
  AutoPop(GUID g, const char *str, const char *prefered_layout=NULL, int layout_required=FALSE) : guid(g), desc(str), layout(prefered_layout), layoutflag(layout_required) { groupid.setValue(NULL); }
  AutoPop(const char *group_id, const char *str, const char *prefered_layout=NULL, int layout_required=FALSE) : groupid(group_id), desc(str), layout(prefered_layout), layoutflag(layout_required) { guid = INVALID_GUID; }

  void onRegisterServices() {
#ifdef WASABI_COMPILE_WNDMGR
    if (guid != INVALID_GUID)
      id = WASABI_API_WNDMGR->autopopup_registerGuid(guid, desc, layout, layoutflag);
    else
      id = WASABI_API_WNDMGR->autopopup_registerGroupId(groupid, desc, layout, layoutflag);
#endif //WASABI_COMPILE_WNDMGR
  }
  void onDeregisterServices() {
#ifdef WASABI_COMPILE_WNDMGR
    WASABI_API_WNDMGR->autopopup_unregister(id);
#endif //WASABI_COMPILE_WNDMGR
  }

private:
  GUID guid;
  String desc;
  String groupid;
  String layout;
  int layoutflag;
  int id;
};


void WAComponentClient::registerAutoPopup(GUID guid, const char *description, const char *prefered_layout, int layout_flag) {
  registerResource(new AutoPop(guid, description, prefered_layout, layout_flag));
}

void WAComponentClient::registerAutoPopup(const char *groupid, const char *description, const char *prefered_layout, int layout_flag) {
  registerResource(new AutoPop(groupid, description, prefered_layout, layout_flag));
}


class CfgItemResource : public LoadableResource {
public:
  CfgItemResource(CfgItemI *_cfgitem, int _autodelete) :
    cfgitem(_cfgitem), autodelete(_autodelete) {}

  virtual void onRegisterServices() {
#ifdef WASABI_API_CONFIG
    WASABI_API_CONFIG->config_registerCfgItem(cfgitem);
#endif
  }

  virtual void onDeregisterServices() {
#ifdef WASABI_API_CONFIG
    WASABI_API_CONFIG->config_deregisterCfgItem(cfgitem);
    if (autodelete) delete cfgitem;
#endif
  }

private:
  CfgItemI *cfgitem;
  int autodelete;
};

void WAComponentClient::registerCfgItem(CfgItemI *cfgitem, int autodelete) {
  registerResource(new CfgItemResource(cfgitem, autodelete));
}


class ExtensionResource : public LoadableResource {
public:
  ExtensionResource(const char *extension, const char *description, const char *family) :
    ext(extension), desc(description), fam(family) { }

  virtual void onRegisterServices() {
#ifdef WASABI_COMPILE_MEDIACORE
    if (!ext.isempty() && !desc.isempty()) WASABI_API_MEDIACORE->core_registerExtension(ext, desc, fam);
#endif
  }
  virtual void onDeregisterServices() {
#ifdef WASABI_COMPILE_MEDIACORE
    if (!ext.isempty() && !desc.isempty()) WASABI_API_MEDIACORE->core_unregisterExtension(ext);
#endif
  }

private:
  String ext, desc, fam;
};

void WAComponentClient::registerExtension(const char *extension, const char *description, const char *family) {
  registerResource(new ExtensionResource(extension, description, family));
}

class CallbackResource : public LoadableResource {
public:
  CallbackResource(SysCallback *_callback, void *_param = NULL) :
      callback(_callback), param(_param) { }

  virtual void onRegisterServices() {
#ifdef WASABI_API_SYSCB
    if (callback) WASABI_API_SYSCB->syscb_registerCallback(callback, param);
#endif
  }
  virtual void onDeregisterServices() {
#ifdef WASABI_API_SYSCB
    if (callback) WASABI_API_SYSCB->syscb_deregisterCallback(callback);
#endif
  }

private:
  SysCallback *callback;
  void *param;
};

class PrefGroupResource : public LoadableResource {
public:
  PrefGroupResource(const char *_groupid, const char *_desc, GUID _guid, GUID _par_guid=INVALID_GUID) : groupid(_groupid), desc(_desc), guid(_guid), par_guid(_par_guid) { }
  virtual void onRegisterServices() {
#ifndef WASABINOMAINAPI
    api->preferences_registerGroup(groupid, desc, guid, par_guid);
#else
//MULTIAPI-FIXME: no preferences api
#endif
  }
  virtual void onDeregisterServices() {
    // someday the api will support deregistration
  }

private:
  String groupid, desc;
  GUID guid, par_guid;
};

void WAComponentClient::registerCallback(SysCallback *_callback, void *_param) {
  registerResource(new CallbackResource(_callback, _param));
}

void WAComponentClient::registerPrefGroup(const char *groupname, const char *dest, GUID pref_guid, GUID pref_guid_parent) {
  registerResource(new PrefGroupResource(groupname, dest, pref_guid, pref_guid_parent));
}

void WAComponentClient::registerResource(LoadableResource *res) {
  ASSERT(!resources.haveItem(res));
  resources.addItem(res);
  if (postregisterservices) res->onRegisterServices(); 
}

void WAComponentClient::internal_onDestroy() {
  onDestroy();
  foreach(resources)
    if (resources.getfor()->deleteOnShutdown()) delete resources.getfor();
  endfor
  resources.removeAll();
}

int WAComponentClient::internal_onNotify(int cmd, int param1, int param2, int param3, int param4) {
  switch (cmd) {
    case WAC_NOTIFY_BEFORELOADINGSKINELEMENTS:
      foreach(resources)
        resources.getfor()->beforeLoadingSkinElements();
      endfor
      break;
    case WAC_NOTIFY_SKINLOADED:
      onSkinLoaded();
      break;
  }
  return onNotify(cmd, param1, param2, param3, param4);
}

//moving wac extern stuff back to here
WAC_GETVERSION
WAC_GET_COMPONENT
WAC_INIT


#define CBCLASS WaComponentI
START_DISPATCH;
  CB(GETNAME, getName);
  CB(GETGUID, getGUID);
#ifndef WASABINOMAINAPI
  VCB(REGISTERSERVICES, registerServices);
#else
  VCB(REGISTERSERVICES2, registerServices);
#endif
  VCB(DEREGISTERSERVICES, deregisterServices);
  VCB(ONCREATE, onCreate);
  VCB(ONDESTROY, internal_onDestroy);
  CB(ONNOTIFY, internal_onNotify);
  CB(ONCOMMAND, onCommand);
#ifdef WASABI_API_CONFIG
  CB(GETCFGINTERFACE, getCfgInterface);
#endif
  VCB(SETOSMODULEHANDLE, setOSModuleHandle);
  CB(GETOSMODULEHANDLE, getOSModuleHandle);
  VCB(SETPATH, setComponentPath);
  CB(GETPATH, getComponentPath);
END_DISPATCH;
#undef CBCLASS

#ifdef WASABINOMAINAPI

api_application *applicationApi = NULL;
api_service *serviceApi = NULL;

#ifdef WASABI_API_SYSCB
api_syscb *sysCallbackApi = NULL;
#endif

#ifdef WASABI_COMPILE_MEMMGR
api_memmgr *memmgrApi = NULL;
#endif

#ifdef WASABI_COMPILE_CONFIG
api_config *configApi = NULL;
#endif

#ifdef WASABI_COMPILE_IMGLDR
imgldr_api *imgLoaderApi = NULL;
#endif

#ifdef WASABI_COMPILE_FONTS
api_font *fontApi = NULL;
#endif

#ifdef WASABI_COMPILE_XMLPARSER
api_xml *xmlApi = NULL;
#endif

#ifdef WASABI_COMPILE_FILEREADER
api_filereader *fileApi = NULL;
#endif

#ifdef WASABI_COMPILE_LOCALES
api_locales *localesApi = NULL;
#endif

#ifdef WASABI_COMPILE_TIMERS
timer_api *timerApi = NULL;
#endif

#ifdef WASABI_COMPILE_WND
wnd_api *wndApi = NULL;
#endif

#ifdef WASABI_COMPILE_WNDMGR
wndmgr_api *wndManagerApi = NULL;
#endif

#ifdef WASABI_COMPILE_COMPONENTS
#endif

#ifdef WASABI_COMPILE_METADB
#endif

#ifdef WASABI_COMPILE_SCRIPT
api_maki *makiApi = NULL;
#endif

#ifdef WASABI_COMPILE_SKIN
api_skin *skinApi;
#endif

#ifdef WASABI_COMPILE_MAKIDEBUG
api_makiDebugger *debugApi = NULL;
#endif

#ifdef WASABI_COMPILE_MEDIACORE
api_core *coreApi = NULL;
#endif

#endif // ifdef WASABINOMAINAPI
