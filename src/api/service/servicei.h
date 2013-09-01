#ifndef _SERVICEI_H
#define _SERVICEI_H

// here is where we define some helper implementations of the service factory
// interface

#include "service.h"
#include <api/api.h>
#include "waservicefactoryi.h"
#include "waservicefactorybase.h"
#include "waservicefactoryt.h"
#include "waservicefactorytsingle.h"
#include <bfc/ptrlist.h>
#include <bfc/foreach.h>

/* CUT
// DEPRECATED
#define waService waServiceFactory
#define waServiceT waServiceFactoryT
#define waServiceTSingle waServiceFactoryTSingle
*/

// Declaring services via macros
// ie:
// BEGIN_SERVICES(YourClass_Svc)
//   DECLARE_SERVICE(SomeSvcCreator<YourServiceClass1>);
//   DECLARE_SERVICE(SomeSvcCreator<YourServiceClass2>);
//   DECLARE_SERVICE(SomeSvcCreator<YourServiceClass3>);
//   DECLARE_SERVICETMULTI(svc_serviceclass, YourMultiInstanceServiceClass1>);
//   DECLARE_SERVICETMULTI(svc_serviceclass, YourMultiInstanceServiceClass2>);
//   DECLARE_SERVICETMULTI(svc_serviceclass, YourMultiInstanceServiceClass3>);
//   DECLARE_SERVICETSINGLE(svc_serviceclass, YourSingleInstanceServiceClass1>);
//   DECLARE_SERVICETSINGLE(svc_serviceclass, YourSingleInstanceServiceClass2>);
//   DECLARE_SERVICETSINGLE(svc_serviceclass, YourSingleInstanceServiceClass3>);
// //  Optional: Declare service module prerequisites
// //    (note: service api will always be present, don't check for it)
// BEGIN_SERVICES_PREREQS
//   PREREQ_BY_GUID(prereq_svc1_guid)
//   PREREQ_BY_GUID(prereq_svc2_guid)
//   PREREQ_BY_GUID(prereq_svc3_guid)
// END_SERVICES_PREREQS
// //  Always:
// END_SERVICES(YourClass_Svc, _YourClass_Svc);

// The macro DECLARE_MODULE_SVCMGR should be defined in your main cpp file if you are compiling
// a standalone exe without using the Application class. In this case you should also add
// MODULE_SVCMGR_ONINIT(); in your app startup and MODULE_SVCMGR_ONSHUTDOWN() in your app shutdown
// (class Application does this already)

// You can use BEGIN_SYS_SERVICES to declare a service that is initialized in priority and 
// shuts down late (normal version inits late and shuts down early)


class StaticServices {
  public:
    virtual void registerServices()=0;
    virtual void unregisterServices()=0;
    virtual int prereqsMet() {return 1; };
};

class AutoServiceList : public PtrList<StaticServices> {
 public:
  AutoServiceList() {
    OutputDebugString(TEXT(">\n"));
  }
};

class StaticServiceMgr {
  public:
    AutoServiceList __m_modules;
    AutoServiceList __m_modules2;
};

/**
  Define the pointer to the static service manager. There must only be
  one of these per binary (main executable or component).
  @see apiinit.cpp
  @see waclient.cpp
*/
#define DECLARE_MODULE_SVCMGR StaticServiceMgr *staticServiceMgr = NULL;

extern StaticServiceMgr *staticServiceMgr;

/**
  Start normal services. Call this second.
  @see MODULE_SVCMGR_SYSTEM_ONINIT
*/
void MODULE_SVCMGR_ONINIT();

/**
  Start system services. Call this first
  @see MODULE_SVCMGR_ONINIT
*/
void MODULE_SVCMGR_SYSTEM_ONINIT();
/** old name */
#define MODULE_SVCMGR_ONINIT2() MODULE_SVCMGR_SYSTEM_ONINIT()

/**
  Shutdown normal services. Call this first.
  @see MODULE_SVCMGR_SYSTEM_ONSHUTDOWN
*/
#define MODULE_SVCMGR_ONSHUTDOWN() { \
foreach(staticServiceMgr->__m_modules) \
  staticServiceMgr->__m_modules.getfor()->unregisterServices(); \
endfor; \
}

/**
  Shutdown system services. Call this last.
  @see MODULE_SVCMGR_ONSHUTDOWN
*/
#define MODULE_SVCMGR_SYSTEM_ONSHUTDOWN() { \
foreach(staticServiceMgr->__m_modules2) \
  staticServiceMgr->__m_modules2.getfor()->unregisterServices(); \
endfor; \
}
/** old name */
#define MODULE_SVCMGR_ONSHUTDOWN2() MODULE_SVCMGR_SYSTEM_ONSHUTDOWN()

/**
  Initialize the static service manager.
*/
#define INIT_MODULE_SVCMGR() \
  if (!staticServiceMgr) staticServiceMgr = new StaticServiceMgr; 

#define SHUTDOWN_MODULE_SVCMGR() \
  delete staticServiceMgr; staticServiceMgr = NULL; 

/**
  Begin a block of service declarations
*/
#define BEGIN_SERVICES(CLASSNAME) \
class CLASSNAME : public StaticServices { \
public: \
  CLASSNAME() { \
    INIT_MODULE_SVCMGR() \
    OutputDebugString(">>>\n"); \
    staticServiceMgr->__m_modules.addItem(this); \
  } \
  virtual ~CLASSNAME() {} \
  virtual void registerServices() { \
  ASSERT(staticServiceMgr);

/**
  Begin a block of service declarations, with debug output.
*/
#define BEGIN_SERVICES_DEBUG(CLASSNAME, TEXT) \
class CLASSNAME : public StaticServices { \
public: \
  CLASSNAME() { \
    INIT_MODULE_SVCMGR() \
    OutputDebugString("Registering "); \
    OutputDebugString(TEXT); \
    OutputDebugString("\n"); \
    staticServiceMgr->__m_modules.addItem(this); \
  } \
  virtual ~CLASSNAME() {} \
  virtual void registerServices() { \
  ASSERT(staticServiceMgr);

/**
  Begin a block of system service declarations.
*/
#define BEGIN_SYS_SERVICES(CLASSNAME) \
class CLASSNAME : public StaticServices { \
public: \
  CLASSNAME() { \
    INIT_MODULE_SVCMGR() \
    staticServiceMgr->__m_modules2.addItem(this); \
} \
  virtual ~CLASSNAME() {} \
  virtual void registerServices() {

#define BEGIN_SYS_SERVICES_DEBUG(CLASSNAME, TEXT) \
class CLASSNAME : public StaticServices { \
public: \
  CLASSNAME() { \
    INIT_MODULE_SVCMGR() \
    OutputDebugString("Registering (System) "); \
    OutputDebugString(TEXT); \
    OutputDebugString("\n"); \
    staticServiceMgr->__m_modules2.addItem(this); \
} \
  virtual ~CLASSNAME() {} \
  virtual void registerServices() {

/**
  Declare a service factory.
*/
#define DECLARE_SERVICE(INSTANTIATIONCODE) \
    registerService(new INSTANTIATIONCODE); 

/**
  Declare a service factory, with debug output
*/
#define DECLARE_SERVICE_DEBUG(INSTANTIATIONCODE, TEXT) \
    OutputDebugString("Starting "); \
    OutputDebugString(TEXT); \
    OutputDebugString("\n"); \
    registerService(new INSTANTIATIONCODE);

/**
  Declare a multiple-instance service.
*/
#define DECLARE_SERVICETMULTI(SVCTYPE, SVCCLASS) \
    registerService(new waServiceFactoryT<SVCTYPE, SVCCLASS>); 

/**
  Declare a multiple-instance service, with debug output.
*/
#define DECLARE_SERVICETMULTI_DEBUG(SVCTYPE, SVCCLASS, TEXT) \
    OutputDebugString("Starting "); \
    OutputDebugString(TEXT); \
    OutputDebugString("\n"); \
    registerService(new waServiceFactoryT<SVCTYPE, SVCCLASS>); 

/**
  Declare a single-instance service.
*/
#define DECLARE_SERVICETSINGLE(SVCTYPE, SVCCLASS) \
    registerService(new waServiceFactoryTSingle<SVCTYPE, SVCCLASS>);

/**
  Declare a single-instance service with a GUID.
 */
#define DECLARE_SERVICETSINGLE_GUID(SVCTYPE, SVCCLASS, GUID) \
    registerService(new waServiceFactoryTSingle<SVCTYPE, SVCCLASS>(GUID));

/**
  Declare a single-instance service, with debug output.
 */
#define DECLARE_SERVICETSINGLE_DEBUG(SVCTYPE, SVCCLASS, TEXT) \
    OutputDebugString("Starting "); \
    OutputDebugString(TEXT); \
    OutputDebugString("\n"); \
    registerService(new waServiceFactoryTSingle<SVCTYPE, SVCCLASS>); 

/**
  Declare a single-instance service with a GUID, with debug output.
 */
#define DECLARE_SERVICETSINGLE_GUID_DEBUG(SVCTYPE, SVCCLASS, GUID, TEXT) \
    OutputDebugString("Starting "); \
    OutputDebugString(TEXT); \
    OutputDebugString("\n"); \
    registerService(new waServiceFactoryTSingle<SVCTYPE, SVCCLASS>(GUID)); 

/**
  Begin declaring Prerequisites for a module of services
 */
#define BEGIN_SERVICES_PREREQS \
  } \
  virtual int prereqsMet() { \

/**
  Declare service module prerequisite by service GUID
*/
#define PREREQ_BY_GUID(PREREQ_SVC_GUID) \
    if(WASABI_API_SVC->service_getServiceByGuid(PREREQ_SVC_GUID) == NULL) return 0;

/**
  End prerequisites block
*/
#define END_SERVICES_PREREQS \
    return 1; \

/**
  End a block of service declarations
*/
#define END_SERVICES(CLASSNAME, INSTANCENAME) \
  } \
  virtual void unregisterServices() { \
    foreach(services) \
      waServiceFactoryI *svc = services.getfor(); \
      WASABI_API_SVC->service_deregister(svc); \
      delete svc; \
    endfor; \
    services.removeAll();\
  } \
private: \
  void registerService(waServiceFactoryI *svc) { \
    if (svc != NULL) { \
      services.addItem(svc); \
      ASSERT(WASABI_API_SVC != NULL); \
      WASABI_API_SVC->service_register(svc); \
    } \
  } \
  PtrList<waServiceFactoryI> services; \
} INSTANCENAME;

// The following macros are used to declare a single service on a single line,
// do not use them in a BEGIN_SERVICES/END_SERVICES block, they create their own

#define _DECLARE_SERVICETSINGLE(SVCTYPE, SVCCLASS) \
BEGIN_SERVICES(SVCCLASS##Svc) \
DECLARE_SERVICETSINGLE(SVCTYPE, SVCCLASS) \
END_SERVICES(SVCCLASS##Svc, _SVCCLASS##Svc)

#define _DECLARE_SERVICETMULTI(SVCTYPE, SVCCLASS) \
BEGIN_SERVICES(SVCCLASS##Svc) \
DECLARE_SERVICETMULTI(SVCTYPE, SVCCLASS) \
END_SERVICES(SVCCLASS##Svc, _SVCCLASS##Svc)

#define _DECLARE_SERVICE(SVCCLASS) \
BEGIN_SERVICES(SVCCLASS##Svc) \
DECLARE_SERVICE(SVCCLASS) \
END_SERVICES(SVCCLASS##Svc, _SVCCLASS##Svc)

#define _DECLARE_SERVICETSINGLE_DEBUG(SVCTYPE, SVCCLASS) \
BEGIN_SERVICES_DEBUG(SVCCLASS##Svc) \
DECLARE_SERVICETSINGLE_DEBUG(SVCTYPE, SVCCLASS) \
END_SERVICES(SVCCLASS##Svc, _SVCCLASS##Svc)

#define _DECLARE_SERVICETMULTI_DEBUG(SVCTYPE, SVCCLASS) \
BEGIN_SERVICES_DEBUG(SVCCLASS##Svc) \
DECLARE_SERVICETMULTI_DEBUG(SVCTYPE, SVCCLASS) \
END_SERVICES(SVCCLASS##Svc, _SVCCLASS##Svc)

#define _DECLARE_SERVICE_DEBUG(SVCCLASS) \
BEGIN_SERVICES_DEBUG(SVCCLASS##Svc) \
DECLARE_SERVICE_DEBUG(SVCCLASS) \
END_SERVICES(SVCCLASS##Svc, _SVCCLASS##Svc)


#endif
