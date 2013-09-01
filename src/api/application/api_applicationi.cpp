#include <precomp.h>
//<?#include "<class data="implementationheader"/>"
#include "api_applicationi.h"
//?>

#include <wasabicfg.h>
#include <api/application/pathmgr.h>
#include <api/application/version.h>
#include <api/apiinit.h>

#ifdef WA3COMPATIBILITY
#include <runtime/main.h> // CUT!
#include <wndmgr/layout.h> // CUT!
#include <skin/skinparse.h>
#endif

#ifdef DARWINBUNDLE
#include  <CoreFoundation/CoreFoundation.h>
#endif

#ifdef WASABI_HTMLHELP
#include <htmlhelp.h>
#endif

String g_resourcepath;

// {3CBD4483-DC44-11d3-B608-000086340885}
static const GUID _baseGUID =
{ 0x3cbd4483, 0xdc44, 0x11d3, { 0xb6, 0x8, 0x0, 0x0, 0x86, 0x34, 0x8, 0x85 } };

api_application *applicationApi = NULL;

api_applicationI::api_applicationI(OSMODULEHANDLE instance) : appInstance(instance), shuttingdown(0) {
#ifndef _WASABIRUNTIME

#ifdef WASABI_DIRS_FROMEXEPATH
  OSMODULEHANDLE hInst = GetModuleHandle(0);
#else
  OSMODULEHANDLE hInst = appInstance;
#endif

  String path;
  {
    char g[WA_MAX_PATH];
#ifndef DARWINBUNDLE
    GetModuleFileName(hInst,g,WA_MAX_PATH-1);
#else
    CFStringRef bid = CFStringCreateWithCString(kCFAllocatorDefault,WasabiVersion::getBundleId(),kCFStringEncodingUTF8);
    CFBundleRef bundle = CFBundleGetBundleWithIdentifier(bid);
    CFRetain(bundle);
    CFURLRef url = CFBundleCopyExecutableURL(bundle);
    CFURLGetFileSystemRepresentation(url,true,g,WA_MAX_PATH-1);
    CFRelease(url);
    CFRelease(bundle);
    CFRelease(bid);
#endif
    char *p = STRRCHR(g, DIRCHAR);
    if (p) *p = 0;
    path = g;
  }
  #ifdef WIN32
    g_resourcepath = path;
  #elif defined(LINUX)
    // FUCKO: this is bad, it needs to be build-time configurable.
    g_resourcepath = "/usr/local/share/";
    g_resourcepath += WasabiVersion::getAppName();
  #endif

  #ifdef LINUX
  //char *file = getenv( "WASABI_LOG_FILE" );
  //if (!ACCESS(file, 0)) UNLINK(file);
  #endif

  apppath = path;

  if (!g_resourcepath.isempty()) g_resourcepath.cat("/");
#if (defined(WASABI_COMPILE_SKIN) || defined(WASABI_COMPILE_IMGLDR)) && defined(WASABI_RESOURCES_SUBDIRECTORY)
  g_resourcepath.cat(WASABI_RESOURCES_SUBDIRECTORY);
#endif

#endif

#ifdef WIN32
  HANDLE h = NULL;
  DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &h, 0, FALSE, DUPLICATE_SAME_ACCESS);
  mainthread = h;  // 32-bit writes assumed atomic
#endif
#ifdef LINUX
  DebugString("portme: api main thread handle dup\n");
#endif
}

api_applicationI::~api_applicationI() {
#ifdef WIN32
  CloseHandle(mainthread);
#endif
  g_resourcepath.purge();
}

const char *api_applicationI::main_getAppName() {
  return WasabiVersion::getAppName();
}

const char *api_applicationI::main_getVersionString() {
  return WasabiVersion::getVersionString();
}

unsigned int api_applicationI::main_getBuildNumber() {
  return WasabiVersion::getBuildNumber();
}

GUID api_applicationI::main_getGUID() {
  return guid;
}

OSTHREADHANDLE api_applicationI::main_getMainThreadHandle() {
  if (mainthread == 0) return (OSTHREADHANDLE)0;
  OSTHREADHANDLE h = (OSTHREADHANDLE)0;
#ifdef WIN32
  DuplicateHandle(GetCurrentProcess(), mainthread, GetCurrentProcess(), &h, 0, FALSE, DUPLICATE_SAME_ACCESS);
#endif
#ifdef LINUX
  DebugString("portme: dup handle\n");
#endif
  return h;
}

OSMODULEHANDLE api_applicationI::main_gethInstance() {
  return appInstance;
}

const char *api_applicationI::main_getCommandLine() {
  return cmdLine;
}

void api_applicationI::main_shutdown(int deferred) {
  shuttingdown = 1;
#ifdef _WASABIRUNTIME
  Main::doAction(ACTION_CLOSE, deferred);
#endif
#ifdef WASABI_CUSTOM_QUIT
WASABI_CUSTOM_QUITFN
#endif

}

void api_applicationI::main_cancelShutdown() {
#ifdef _WASABIRUNTIME
  Main::cancelShutdown();
#endif
  shuttingdown = 0;
}

int api_applicationI::main_isShuttingDown() {
  return shuttingdown;
}

const char *api_applicationI::path_getAppPath() {
#ifdef _WASABIRUNTIME
  return Main::getMainAppPath();
#endif
  return apppath;
}

void api_applicationI::path_setAppPath(const char *path) {
#ifdef _WASABIRUNTIME
  //todo: implement Main::setMainAppPath(path);
#endif
  apppath = path;
}

const char *api_applicationI::path_getUserSettingsPath() {
  return PathMgr::getUserSettingsPath();
}

const char *api_applicationI::path_getOriginalPath() {
  return PathMgr::getOriginalPath();
}

void api_applicationI::setHInstance(OSMODULEHANDLE instance) {
  appInstance = instance;
};

void api_applicationI::setCommandLine(const char *cmdline) {
  cmdLine = cmdline;
}

void api_applicationI::setGUID(GUID g) {
  guid = g;
}

int api_applicationI::app_getInitCount() {
  return ApiInit::getInitCount();
}


int api_applicationI::app_messageLoopStep() {
#if defined(WASABI_COMPILE_TIMERS) || defined(WASABI_COMPILE_WND)
  MSG msg;
  if (PeekMessage(&msg, INVALIDOSWINDOWHANDLE, 0, 0, PM_NOREMOVE)) {
    if (!GetMessage( &msg, INVALIDOSWINDOWHANDLE, 0, 0 )) return 0;
#ifdef WASABI_HTMLHELP
    if (!HtmlHelp(NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD)&msg)) {
#endif
#ifdef WASABI_MDI
      if (g_mdi) {
        if (TranslateMDISysAccel(g_mdi, &msg)) return 1;
      }
#endif
      TranslateMessage( &msg );
      DispatchMessage( &msg );
#ifdef WASABI_HTMLHELP
    }
#endif
  }
#endif
  return 1;
}

