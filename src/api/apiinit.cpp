#include <precomp.h>
//CUT #include <api/api.h>

//<?#include "<class data="implementationheader"/>"
#include "apiinit.h"
//?>


#include <api/service/servicei.h>
#include <api/service/svcenumbyguid.h>

#ifdef WASABI_COMPILE_SYSCB
#include <api/syscb/api_syscbi.h>
#endif

#ifdef WASABI_COMPILE_LINUX
#include <api/linux/api_linuxi.h>
#endif

#ifdef WASABI_COMPILE_CONFIG
#include <api/config/api_configi.h>
#endif

#ifdef WASABI_COMPILE_FONTS
#include <api/font/fontapi.h>
#endif

#ifdef WASABI_COMPILE_TIMERS
#include <api/timer/timerapi.h>
#endif

#ifdef WASABI_COMPILE_XMLPARSER
#include <api/xml/xmlapi.h>
#endif

#ifdef WASABI_COMPILE_LOCALES
#include <api/locales/api_localesI.h>
#endif

#ifdef WASABI_COMPILE_MEMMGR
#include <api/memmgr/api_memmgrI.h>
#endif

#ifdef WASABI_COMPILE_FILEREADER
#include <api/filereader/api_filereaderI.h>
#endif

#ifdef WASABI_COMPILE_MEDIACORE
# include <api/core/core_api.h>
#endif

#ifdef WASABI_COMPILE_WND
#include <api/wnd/wndapi.h>
#endif

#ifdef WASABI_COMPILE_IMGLDR
#include <img/imgldrapi.h>
#ifdef WASABI_COMPILE_IMGLDR_PNGREAD
#include <img/pngload/loader.h>
#endif
#endif

#ifdef WASABI_COMPILE_SKIN
#include <api/skin/skinapi.h>
#endif

#ifdef WASABI_COMPILE_FONTS
#ifdef WIN32
# include <api/font/win32/truetypefont_win32.h>
#elif defined(LINUX)
# include <api/font/linux/truetypefont_linux.h>
#else
# error port me
#endif // platform
#endif // fonts

#ifndef WASABI_CUSTOM_MODULE_SVCMGR
DECLARE_MODULE_SVCMGR
#endif

BEGIN_SYS_SERVICES_DEBUG(KernelServices, "Kernel API Services")
  #ifdef WASABI_COMPILE_SYSCB
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_syscb, api_syscbI, syscbApiServiceGuid, api_syscbI::getServiceName())
    WASABI_API_SYSCB = SvcEnumByGuid<api_syscb>(syscbApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_LINUX
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_linux, api_linuxI, linuxApiServiceGuid, "Linux Platform API")
    WASABI_API_LINUX = SvcEnumByGuid<api_linux>(linuxApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_CONFIG
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_config, api_configI, configApiServiceGuid, api_configI::getServiceName())
    WASABI_API_CONFIG = SvcEnumByGuid<api_config>(configApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_IMGLDR
    DECLARE_SERVICETSINGLE_GUID_DEBUG(imgldr_api, ImgLdrApi, imgLdrApiServiceGuid, ImgLdrApi::getServiceName())
    WASABI_API_IMGLDR = SvcEnumByGuid<imgldr_api>(imgLdrApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_FONTS
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_font, FontApi, fontApiServiceGuid, FontApi::getServiceName())
    DECLARE_SERVICE_DEBUG(FontCreator<TrueTypeFont_Win32>, "  Win32 TrueType font creator")
    WASABI_API_FONT = SvcEnumByGuid<api_font>(fontApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_XMLPARSER
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_xml, XmlApi, xmlApiServiceGuid, XmlApi::getServiceName())
    WASABI_API_XML = SvcEnumByGuid<api_xml>(xmlApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_TIMERS
    DECLARE_SERVICETSINGLE_GUID_DEBUG(timer_api, TimerApi, timerApiServiceGuid, TimerApi::getServiceName())
    WASABI_API_TIMER = SvcEnumByGuid<timer_api>(timerApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_JAVASCRIPT
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_js, api_jsI, JSApiServiceGuid, api_jsI::getServiceName())
    WASABI_API_JS = SvcEnumByGuid<api_js>(JSApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_DATAPUMP
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_pump, api_pumpI, pumpApiServiceGuid, api_pumpI::getServiceName())
    WASABI_API_DATAPUMP = SvcEnumByGuid<api_pump>(pumpApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_LOCALES
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_locales, api_localesI, localesGuid, api_localesI::getServiceName())
    WASABI_API_LOCALE = SvcEnumByGuid<api_locales>(localesGuid);
  #endif
  #ifdef WASABI_COMPILE_FILEREADER
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_filereader, api_filereaderI, fileApiServiceGuid, api_filereaderI::getServiceName())
    WASABI_API_FILE = SvcEnumByGuid<api_filereader>(fileApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_MEMMGR
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_memmgr, api_memmgrI, memmgrApiServiceGuid, api_memmgrI::getServiceName())
    WASABI_API_MEMMGR = SvcEnumByGuid<api_memmgr>(memmgrApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_MEDIACORE
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_core, core_api, coreApiServiceGuid, core_api::getServiceName())
    WASABI_API_MEDIACORE = SvcEnumByGuid<api_core>(coreApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_WND
    DECLARE_SERVICETSINGLE_GUID_DEBUG(wnd_api, wndapi, wndApiServiceGuid, wndapi::getServiceName())
    WASABI_API_WND = SvcEnumByGuid<wnd_api>(wndApiServiceGuid);
  #endif
  #ifdef WASABI_COMPILE_SKIN
    DECLARE_SERVICETSINGLE_GUID_DEBUG(api_skin, skinApiI, skinApiServiceGuid, skinApiI::getServiceName())
    WASABI_API_SKIN = SvcEnumByGuid<api_skin>(skinApiServiceGuid);    
  #endif
  //NOTE: WaI> I removed the code for old APIs we don't have source for, but they are easy enough to add back.
  //           Just be sure to use the old names from apiconfig.h.
END_SERVICES(KernelServices, kernelServices)

api_serviceI *g_api;

int ApiInit::init(OSMODULEHANDLE hinstance, GUID app_guid, const char *cmd_line, OSWINDOWHANDLE mainwnd) {

  initcount++;

  // create Application API
  app = new api_applicationI(hinstance);
  WASABI_API_APP = app;

  app->setCommandLine(cmd_line);
  app->setGUID(app_guid);

  // create Service API
  svc = new api_serviceI();
  WASABI_API_SVC = svc;
  g_api = svc;

  // register Application API
  svc->service_register(appapisvc = new ApplicationApiService());


  // if no static service was ever declared, we need to init it so we don't hit null
  INIT_MODULE_SVCMGR();

  DebugString("Auto-initializing %d service modules (system pass)\n", staticServiceMgr->__m_modules2.getNumItems());
  MODULE_SVCMGR_SYSTEM_ONINIT();

  DebugString("Auto-initializing %d service modules (user pass)\n", staticServiceMgr->__m_modules.getNumItems());
  MODULE_SVCMGR_ONINIT();

  return 1; // success
}

int ApiInit::shutdown() {

  MODULE_SVCMGR_ONSHUTDOWN();

  MODULE_SVCMGR_SYSTEM_ONSHUTDOWN();

  delete appapisvc; appapisvc = NULL;

  delete svc; svc = NULL;
  WASABI_API_SVC = NULL;
  g_api = NULL;

  delete app; app = NULL;
  SHUTDOWN_MODULE_SVCMGR();
  //FUCKO: do the rest
  //FUCKO: should we not delete these? are we sure the factory has deleted them?
  #ifdef WASABI_COMPILE_SYSCB
    WASABI_API_SYSCB = NULL;
  #endif
  #ifdef WASABI_COMPILE_LINUX
    WASABI_API_LINUX = NULL;
  #endif
  #ifdef WASABI_COMPILE_CONFIG
    WASABI_API_CONFIG = NULL;
  #endif
  #ifdef WASABI_COMPILE_TIMERS
    WASABI_API_TIMER = NULL;
  #endif

  return 1; // success
}

int ApiInit::getInitCount() {
  return initcount;
}


api_applicationI * ApiInit::app = NULL;
ApplicationApiService *ApiInit::appapisvc = NULL;

api_serviceI * ApiInit::svc = NULL;

int ApiInit::initcount = 0;
