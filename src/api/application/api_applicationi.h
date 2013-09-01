#ifndef __API_APPLICATIONI_IMPL_H
#define __API_APPLICATIONI_IMPL_H

/*<?<autoheader/>*/
#include "api_application.h"
#include "api_applicationx.h"
/*?>*/

/*[interface.header.h]
#include "common/nsGUID.h"
class String;
*/

#include <api/service/services.h>

class api_applicationI : public api_applicationX {
public:
  NODISPATCH api_applicationI(OSMODULEHANDLE instance);
  NODISPATCH virtual ~api_applicationI();

  static const char *getServiceName() { return "Application API"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }
  
  DISPATCH(10) const char *main_getAppName();
  DISPATCH(20) const char *main_getVersionString();
  DISPATCH(30) unsigned int main_getBuildNumber();
  DISPATCH(40) GUID main_getGUID();
  DISPATCH(50) OSTHREADHANDLE main_getMainThreadHandle();
  DISPATCH(60) OSMODULEHANDLE main_gethInstance();
  DISPATCH(70) const char *main_getCommandLine();
  DISPATCH(80) void main_shutdown(int deferred = TRUE);
  DISPATCH(90) void main_cancelShutdown();
  DISPATCH(100) int main_isShuttingDown();
  DISPATCH(110) const char *path_getAppPath();
  DISPATCH(111) void path_setAppPath(const char *path);
  DISPATCH(120) const char *path_getUserSettingsPath();
  DISPATCH(125) const char *path_getOriginalPath();
  DISPATCH(130) int app_getInitCount();
  DISPATCH(140) int app_messageLoopStep();

  NODISPATCH void setHInstance(OSMODULEHANDLE instance);
  NODISPATCH void setCommandLine(const char *cmdline);
  NODISPATCH void setGUID(GUID g);

protected:
    
  OSMODULEHANDLE appInstance;
  String cmdLine;
  OSTHREADHANDLE mainthread;
  GUID guid;
  int shuttingdown;
  String apppath;
};

/*[interface.footer.h]
// {23B96771-09D7-46d3-9AE2-20DCEA6C86EA}
static const GUID applicationApiServiceGuid = 
{ 0x23b96771, 0x9d7, 0x46d3, { 0x9a, 0xe2, 0x20, 0xdc, 0xea, 0x6c, 0x86, 0xea } };

extern api_application *applicationApi;
extern String g_resourcepath;
*/


#endif // __API_APPLICATIONI_IMPL_H
