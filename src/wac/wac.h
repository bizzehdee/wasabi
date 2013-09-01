#ifndef _WAC_H
#define _WAC_H

//biz> do we still need api/api.h?
#include <api/api.h>//CUT
#include <bfc/ptrlist.h>
#include <bfc/attrib/cfgitemi.h>

class CanvasBase;	// see ../common/canvas.h
#ifdef WASABI_API_CONFIG
class CfgItem;		// see ../attribs/cfgitem
class CfgItemI;
#endif
#ifndef WASABINOMAINAPI
class ComponentAPI;	// see api.h
#endif
class RootWnd;		// see ../bfc/rootwnd.h
class LoadableResource;	// see below

// this tells wasabi what version of the SDK we're compiled with
// this number will be incremented every once in a while. when it is, you'll
// have to recompile

// 5 -> build #471
// 6 -> build rc1	(most recent)
// 7 -> build 484 (rc5)
// 8 -> all new standalone wasabi
#define WA_COMPONENT_VERSION 8

#define DB_DENY  0
#define DB_ALLOW 1

// commands for onNotify below

#define WAC_NOTIFY_NOP			0

#define WAC_NOTIFY_ACTIVATE		10
#define WAC_NOTIFY_DEACTIVATE		20

#define WAC_NOTIFY_FULLSCREEN		30
#define WAC_NOTIFY_RESTORE		40

#define WAC_NOTIFY_ENTERRESIZE		50
#define WAC_NOTIFY_LEAVERESIZE		51

#define WAC_NOTIFY_SYSTRAYDONE		60
#define WAC_NOTIFY_POPUPDONE  		61

#define WAC_NOTIFY_BASETEXTUREWINDOW	78

#define WAC_NOTIFY_LOADEARLY		90

#define WAC_NOTIFY_MENU_COMMAND		10000

#define WAC_NOTIFY_LOAD_SELF 30000
#define WAC_NOTIFY_UNLOAD_SELF 30001

//param = wac guid
#define WAC_NOTIFY_LOAD_OTHER 30010
#define WAC_NOTIFY_UNLOAD_OTHER 30011

// this message is sent when service runlevel notifies are sent
// param1 = msg
// param2 = param1
// param3 = param2
#define WAC_NOTIFY_SERVICE_NOTIFY	100

#define WAC_NOTIFY_SKINUNLOADING             0xE300 // before unloading current skin
#define WAC_NOTIFY_SWITCHINGSKIN             0xE301 // right after unloading current skin and right before loading new one, getSkinName/Path points to new skin
#define WAC_NOTIFY_SKINELEMENTSLOADED	       0xE302 // after skin elements definitions are loaded
#define WAC_NOTIFY_BEFORELOADINGSKINELEMENTS 0xE303 // before skin elements definitions are loaded, trap this to load your elements/groups and let people override them
#define WAC_NOTIFY_SKINGUILOADED	           0xE304 // after skin GUI objects are loaded from xml
#define WAC_NOTIFY_SKINLOADED	               0xE305 // after skin is fully loaded

enum {
  RSF_STATIC=0,
  RSF_RELATIVETOWAC=1,
  RSF_RELATIVETOTHEME=2,
};

// -----------

#include "bfc/dispatch.h"

class NOVTABLE WaComponent : public Dispatchable {
public:
  // provide your component name & other info here
  const char *getName() { return _call(GETNAME, (char *)NULL); }
  GUID getGUID() { return _call(GETGUID, INVALID_GUID); }

#ifdef WASABINOMAINAPI
  void registerServices(api_service *a) { _voidcall(REGISTERSERVICES2, a); }
#else
  void registerServices(ComponentAPI *a) { _voidcall(REGISTERSERVICES, a); }
#endif
  void deregisterServices() { _voidcall(DEREGISTERSERVICES); }

  void onCreate() { _voidcall(ONCREATE); }
  void onDestroy() { _voidcall(ONDESTROY); }

  int onNotify(int cmd, int param1=0, int param2=0, int param3=0, int param4=0){
    return _call(ONNOTIFY, 0, cmd, param1, param2, param3, param4);
  }
  // everything after cmd is for future expansion
  int onCommand(const char *cmd, int param1, int param2, void *ptr, int ptrlen){
    return _call(ONCOMMAND, 0, cmd, param1, param2, ptr, ptrlen);
  }

  // gets the CfgItem * from WAComponentClient
  CfgItem *getCfgInterface(int n) {
    return _call(GETCFGINTERFACE, (CfgItem*)NULL, n);
  }

  // saves the OSMODULEHANDLE of your WAC for you
  void setOSModuleHandle(OSMODULEHANDLE modulehandle) {
    _voidcall(SETOSMODULEHANDLE, modulehandle);
  }
  OSMODULEHANDLE getOSModuleHandle() {
    return _call(GETOSMODULEHANDLE, (OSMODULEHANDLE)0);
  }
  // saves the path of your WAC for you
  void setComponentPath(const char *path) {
    _voidcall(SETPATH, path);
  }
  const char *getComponentPath() {
    return _call(GETPATH, (const char *)NULL);
  }

  enum {
    GETNAME=100,
    GETGUID=120,
    REGISTERSERVICES=200,
    REGISTERSERVICES2=210,
    ONCREATE=400,
    ONDESTROY=410,
    ONNOTIFY=500,
    ONCOMMAND=600,
    GETCFGINTERFACE=800,
    SETHINSTANCE=900, SETOSMODULEHANDLE=900, // note dups for back-compat
    GETHINSTANCE=910, GETOSMODULEHANDLE=910, // note dups for back-compat
    DEREGISTERSERVICES=1000,
    SETPATH=1100,
    GETPATH=1200, 
  };
};

// hides the dispatchable interface
class NOVTABLE WaComponentI : public WaComponent {
protected:
  WaComponentI() {}	// protect constructor
public:
  virtual const char *getName()=0;
  virtual GUID getGUID()=0;

#ifdef WASABINOMAINAPI
  virtual void registerServices(api_service *)=0;
#else
  virtual void registerServices(ComponentAPI *)=0;
#endif
  virtual void deregisterServices()=0;

  virtual void onCreate()=0;
  virtual void onDestroy()=0;

  virtual int onNotify(int cmd, int param1, int param2, int param3, int param4)=0;
  virtual int onCommand(const char *cmd, int param1, int param2, void *ptr, int ptrlen)=0;

  virtual CfgItem *getCfgInterface(int n)=0;

  virtual void setOSModuleHandle(OSMODULEHANDLE moduleHandle)=0;
  virtual OSMODULEHANDLE getOSModuleHandle()=0;
  virtual void setComponentPath(const char *path)=0;
  virtual const char *getComponentPath()=0;

private:
  virtual void internal_onDestroy()=0;
  virtual int internal_onNotify(int cmd, int param1, int param2, int param3, int param4)=0;	// calls thru to onNotify after processing
  RECVS_DISPATCH;
};

class waServiceFactoryI;

/**
*/
class NOVTABLE WAComponentClient : public WaComponentI
#ifdef WASABI_API_CONFIG
, public CfgItemI 
#endif
{
protected:
  WAComponentClient(const char *name=NULL);
public:
  virtual ~WAComponentClient() {}

  // provide your component name & other info here
  virtual const char *getName(); //OVERRIDE ME (or just set name in constructor)
  virtual GUID getGUID()=0;	// be sure to override this one	//OVERRIDE ME

#ifdef WASABINOMAINAPI
  void registerServices(api_service *);	// don't override
#else
  void registerServices(ComponentAPI *);	// don't override
#endif //WASABINOMAINAPI

  void deregisterServices();			// don't override

  // override these to receive notifications
  virtual void onRegisterServices() {}// register extra services here
  virtual void onCreate() {}	// init stuff
  virtual void onDestroy() {}	// destroy everything here, not in ~

  // OVERRIDE ME for various events
  virtual int onNotify(int cmd, int param1, int param2, int param3, int param4) { return 0; }
  // everything after cmd is for future expansion
  virtual int onCommand(const char *cmd, int param1, int param2, void *ptr, int ptrlen) { return 0; }

  // config ptr... you can override this and provide a different *
  // otherwise 'this' will be returned
  virtual CfgItem *getCfgInterface(int n);

  // saves the OSMODULEHANDLE of your WAC for you
  void setOSModuleHandle(OSMODULEHANDLE moduleHandle) { OSModuleHandle = moduleHandle; };
  OSMODULEHANDLE getOSModuleHandle() { return OSModuleHandle; }
  void setComponentPath(const char *path) { componentpath = path; };
  const char *getComponentPath() { return componentpath; }
  OSMODULEHANDLE gethInstance() { return getOSModuleHandle(); } //DEPRECATED

  // various options to register during your constructor, like services,
  // skin parts, and autopop guids. just register them and forget them

  void registerService(waServiceFactoryI* service);
  void registerSkinFile(const char *filename, int relative=RSF_RELATIVETOWAC);
  void registerAutoPopup(GUID guid, const char *description, const char *prefered_container=NULL, int container_required=FALSE);
  void registerAutoPopup(const char *groupid, const char *description, const char *container_layout=NULL, int container_required=FALSE);
  void registerCfgItem(CfgItemI *cfgitem, int autodelete=FALSE);
  void registerExtension(const char *extension, const char *description, const char *family=NULL);
  void registerCallback(SysCallback *_callback, void *_param = NULL);
  void registerPrefGroup(const char *groupname, const char *dest, GUID pref_guid, GUID pref_guid_parent=INVALID_GUID);
  // or register your own resource types here
  void registerResource(LoadableResource *res);

  virtual void onSkinLoaded() {}

private:
  virtual void internal_onDestroy();
  virtual int internal_onNotify(int cmd, int param1, int param2, int param3, int param4);
  OSMODULEHANDLE OSModuleHandle;
  int postregisterservices;	// if 1, onRegisterServices has been called
  PtrList<LoadableResource> resources;
  String componentpath;
};

#ifndef STATIC_COMPONENT
extern WAComponentClient *the;

#ifndef WASABINOMAINAPI
ComponentAPI *api;	// we do NOT delete this on shutdown!!!
#endif

static OSMODULEHANDLE g_modulehandle;
#endif	//STATIC_COMPONENT

#define WAC_EXTERN_STRONG(WCC) \
extern WCC *the;

#define WAC_EXTERN() \
WAC_EXTERN_STRONG(WAComponentClient)

#define WAC_DEFINE_STRONG(WCLASS,WCC) \
static WCLASS wac; \
WCC *the = &wac;

#define WAC_DEFINE(WCLASS) \
WAC_DEFINE_STRONG(WCLASS, WAComponentClient)

#define WAC_GETVERSION \
extern "C" { \
  __declspec(dllexport) UINT WAC_getVersion(void) { \
    return WA_COMPONENT_VERSION; \
} \
}

#define WAC_GET_COMPONENT \
extern "C" { \
  __declspec(dllexport) WaComponent *WAC_getWaComponent(void) { \
    return the; \
} \
}

#define WAC_INIT \
extern "C" { \
  __declspec(dllexport) void WAC_init(OSMODULEHANDLE moduleHandle) { \
    if (g_modulehandle == NULL) g_modulehandle = moduleHandle; \
    the->setOSModuleHandle(moduleHandle); \
} \
}

extern "C" {
  typedef void (*WACINIT)(OSMODULEHANDLE);
  typedef UINT (*WACGETVERSION)(void); // returns the interface version component was compiled with
  typedef WaComponent *(*WACGETCOMPONENT)(void);
};

class LoadableResource {
public:
  virtual ~LoadableResource() {}

  virtual int deleteOnShutdown() { return TRUE; } // return FALSE if you're static

  virtual void onRegisterServices() {}
  virtual void onDeregisterServices() {}	// unload during this one
  virtual void beforeLoadingSkinElements() {}
};

#endif
