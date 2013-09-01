// ----------------------------------------------------------------------------
// Generated by InterfaceFactory [Wed May 07 00:55:56 2003]
// 
// File        : api_application.h
// Class       : api_application
// class layer : Dispatchable Interface
// ----------------------------------------------------------------------------

#ifndef __API_APPLICATION_H
#define __API_APPLICATION_H

#include <bfc/dispatch.h>
#include <bfc/common.h>
#include <bfc/nsguid.h>

class String;

// ----------------------------------------------------------------------------

class api_application: public Dispatchable {
  protected:
    api_application() {}
    ~api_application() {}
  public:
    const char *main_getAppName();
    const char *main_getVersionString();
    unsigned int main_getBuildNumber();
    GUID main_getGUID();
    OSTHREADHANDLE main_getMainThreadHandle();
    OSMODULEHANDLE main_gethInstance();
    const char *main_getCommandLine();
    void main_shutdown(int deferred = TRUE);
    void main_cancelShutdown();
    int main_isShuttingDown();
    const char *path_getAppPath();
    void path_setAppPath(const char *path);
    const char *path_getUserSettingsPath();
    const char *path_getOriginalPath();
    int app_getInitCount();
    int app_messageLoopStep();
  
  protected:
    enum {
      API_APPLICATION_MAIN_GETAPPNAME = 10,
      API_APPLICATION_MAIN_GETVERSIONSTRING = 20,
      API_APPLICATION_MAIN_GETBUILDNUMBER = 30,
      API_APPLICATION_MAIN_GETGUID = 40,
      API_APPLICATION_MAIN_GETMAINTHREADHANDLE = 50,
      API_APPLICATION_MAIN_GETHINSTANCE = 60,
      API_APPLICATION_MAIN_GETCOMMANDLINE = 70,
      API_APPLICATION_MAIN_SHUTDOWN = 80,
      API_APPLICATION_MAIN_CANCELSHUTDOWN = 90,
      API_APPLICATION_MAIN_ISSHUTTINGDOWN = 100,
      API_APPLICATION_PATH_GETAPPPATH = 110,
      API_APPLICATION_PATH_SETAPPPATH = 111,
      API_APPLICATION_PATH_GETUSERSETTINGSPATH = 120,
      API_APPLICATION_PATH_GETORIGINALPATH = 125,
      API_APPLICATION_APP_GETINITCOUNT = 130,
      API_APPLICATION_APP_MESSAGELOOPSTEP = 140,
    };
};

// ----------------------------------------------------------------------------

inline const char *api_application::main_getAppName() {
  const char *__retval = _call(API_APPLICATION_MAIN_GETAPPNAME, (const char *)0);
  return __retval;
}

inline const char *api_application::main_getVersionString() {
  const char *__retval = _call(API_APPLICATION_MAIN_GETVERSIONSTRING, (const char *)0);
  return __retval;
}

inline unsigned int api_application::main_getBuildNumber() {
  unsigned int __retval = _call(API_APPLICATION_MAIN_GETBUILDNUMBER, (unsigned int)0);
  return __retval;
}

inline GUID api_application::main_getGUID() {
  GUID __retval = _call(API_APPLICATION_MAIN_GETGUID, INVALID_GUID);
  return __retval;
}

inline OSTHREADHANDLE api_application::main_getMainThreadHandle() {
  OSTHREADHANDLE __retval = _call(API_APPLICATION_MAIN_GETMAINTHREADHANDLE, (OSTHREADHANDLE)NULL);
  return __retval;
}

inline OSMODULEHANDLE api_application::main_gethInstance() {
  OSMODULEHANDLE __retval = _call(API_APPLICATION_MAIN_GETHINSTANCE, INVALIDOSMODULEHANDLE);
  return __retval;
}

inline const char *api_application::main_getCommandLine() {
  const char *__retval = _call(API_APPLICATION_MAIN_GETCOMMANDLINE, (const char *)0);
  return __retval;
}

inline void api_application::main_shutdown(int deferred) {
  _voidcall(API_APPLICATION_MAIN_SHUTDOWN, deferred);
}

inline void api_application::main_cancelShutdown() {
  _voidcall(API_APPLICATION_MAIN_CANCELSHUTDOWN);
}

inline int api_application::main_isShuttingDown() {
  int __retval = _call(API_APPLICATION_MAIN_ISSHUTTINGDOWN, (int)0);
  return __retval;
}

inline const char *api_application::path_getAppPath() {
  const char *__retval = _call(API_APPLICATION_PATH_GETAPPPATH, (const char *)0);
  return __retval;
}

inline void api_application::path_setAppPath(const char *path) {
  _voidcall(API_APPLICATION_PATH_SETAPPPATH, path);
}

inline const char *api_application::path_getUserSettingsPath() {
  const char *__retval = _call(API_APPLICATION_PATH_GETUSERSETTINGSPATH, (const char *)0);
  return __retval;
}

inline const char *api_application::path_getOriginalPath() {
  const char *__retval = _call(API_APPLICATION_PATH_GETORIGINALPATH, (const char *)0);
  return __retval;
}

inline int api_application::app_getInitCount() {
  int __retval = _call(API_APPLICATION_APP_GETINITCOUNT, 0);
  return __retval;
}

inline int api_application::app_messageLoopStep() {
  int __retVal = _call(API_APPLICATION_APP_MESSAGELOOPSTEP, 1);
  return __retVal;
}

// ----------------------------------------------------------------------------


// {23B96771-09D7-46d3-9AE2-20DCEA6C86EA}
static const GUID applicationApiServiceGuid = 
{ 0x23b96771, 0x9d7, 0x46d3, { 0x9a, 0xe2, 0x20, 0xdc, 0xea, 0x6c, 0x86, 0xea } };

extern api_application *applicationApi;
extern String g_resourcepath;

#endif // __API_APPLICATION_H
