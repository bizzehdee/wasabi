#ifndef __WASERVICEFACTORYBASE_IMPL_H
#define __WASERVICEFACTORYBASE_IMPL_H

/*<?<autoheader/>*/
class CfgItem;
/*?>*/

#include "waservicefactoryi.h"

/**
  Intermidiate Factory class to derive from. Provides partial default implementation, including
  storing a Factory GUID, and reporting service name from template. You probably don't want to
  use this.
  Only missing get and release methods.
  @see waServiceFactoryBase
  @see waServiceBase
*/
template <class SERVICETYPE, class SERVICE>
class NOVTABLE waServiceFactoryBaseX : public waServiceFactoryI {
public:
  waServiceFactoryBaseX(GUID myGuid = INVALID_GUID) : guid(myGuid) {}
  virtual GUID svc_serviceType()=0;
  virtual const char *svc_getServiceName() { return SERVICE::getServiceName(); }
  virtual GUID svc_getGuid() { return guid; }
  virtual void *svc_getInterfaceAndLock() {// this is only for back compat
    return getInterface(TRUE);
  }

  virtual void *svc_getInterface(int global_lock)=0;
  virtual int svc_releaseInterface(void *ptr)=0;

  virtual CfgItem *svc_getCfgInterface() { return NULL; }
  virtual const char *svc_getTestString() { return NULL; }

  virtual int svc_notify(int msg, int param1 = 0, int param2 = 0) { return 0; }

private:
  GUID guid;
};

/**
  Basic Factory base class. Override newService() and delService() to create a complete Factory.
  If all you need to do is instantiate a service and destroy it, use the waServiceFactoryT helper
  @see waServiceFactoryT
  @see waServiceFactoryTSingle
*/
template <class SERVICETYPE, class SERVICE>
class NOVTABLE waServiceFactoryBase : public waServiceFactoryBaseX<SERVICETYPE, SERVICE> {
public:
  waServiceFactoryBase(GUID myGuid = INVALID_GUID) : waServiceFactoryBaseX<SERVICETYPE, SERVICE>(myGuid) {}
  virtual GUID svc_serviceType() { return SERVICE::getServiceType(); } //WaI>this would probably make more sense to be SERVICETYPE, but the APIs weren't made that way.
  virtual void *svc_getInterface(int global_lock) { // new style, client optionally does the locking
    SERVICETYPE *ret = newService();
    if (global_lock) WASABI_API_SVC->service_lock(this, ret);
    return ret;
  }
  virtual int svc_releaseInterface(void *ptr) {
    return delService(static_cast<SERVICE*>(static_cast<SERVICETYPE*>(ptr)));
  }
protected:
  virtual SERVICETYPE *newService()=0;
  virtual int delService(SERVICETYPE *service)=0;
};

/**
  Basic "factory" base class to expose a single service pointer without factory. Override
  newService() to create a complete Factory. Note that SERVICETYPE and SERVICE do not
  have to be related (unlike waServiceFactoryBase which needs a relationship between
  the classes in releaseInterface)
  @see apiinit.h
*/
template <class SERVICETYPE, class SERVICE>
class NOVTABLE waServiceBase : public waServiceFactoryBaseX<SERVICETYPE, SERVICE> {
public:
  waServiceBase(GUID myGuid = INVALID_GUID) : waServiceFactoryBaseX<SERVICETYPE, SERVICE>(myGuid) {}
  virtual GUID svc_serviceType() { return SERVICE::getServiceType(); }
  virtual void *svc_getInterface(int global_lock) { // new style, client optionally does the locking
    SERVICETYPE *ret = getService();
    if (global_lock) WASABI_API_SVC->service_lock(this, ret);
    return ret;
  }
  virtual int svc_releaseInterface(void *ptr) {
    return TRUE;
  }
protected:
  virtual SERVICETYPE *getService()=0;
};



#endif // __WASERVICEFACTORYBASE_IMPL_H
