#ifndef __API_INIT_H
#define __API_INIT_H
/**
  Loading and initialization of statically loaded/linked kernel APIs
*/

//CUT #include <api/api.h>
#include <bfc/platform/platform.h>
#include <api/service/waservicefactorytsingle.h>

#include <api/application/api_applicationi.h>
#include <api/service/api_servicei.h>



/**
  Because the Application API requires an argument (the hInstance), and is created before
  the service API, it must be created before it's Factory. This class achieves that by
  returning an external pointer.
*/
class ApplicationApiService : public waServiceBase<api_application, api_applicationI> {
  public:
    ApplicationApiService() : waServiceBase<api_application, api_applicationI>(applicationApiServiceGuid) {}
    virtual api_application *getService() { return applicationApi; }
};


class ApiInit {
public:
  static int init(OSMODULEHANDLE hinstance, GUID app_guid, const char *cmd_line, OSWINDOWHANDLE mainwnd = NULL); // 0 == error, 1 == success
  static int shutdown(); // 0 == error, 1 == success
  static int getInitCount(); // if you do init / shutdown more than once in the session, this may be useful to invalidate some caches

private:

  static api_applicationI * app;
  static ApplicationApiService *appapisvc;

  static api_serviceI * svc;

  static int initcount;
};

#endif

