#include <precomp.h>
//<?#include "<class data="implementationheader"/>"
#include "api_syscbi.h"
//?>

#include <api/api.h>
#include <api/syscb/cbmgr.h>

api_syscb *sysCallbackApi = NULL;

int api_syscbI::syscb_registerCallback(SysCallback *cb, void *param) {
#ifdef WASABI_COMPILE_COMPONENTS
  CallbackManager::registerCallback(cb, param, WASABI_API_COMPONENT->getThisComponent());
#else
  CallbackManager::registerCallback(cb, param, NULL);
#endif
  return 1;
}

int api_syscbI::syscb_deregisterCallback(SysCallback *cb) {
#ifdef WASABI_COMPILE_COMPONENTS
  CallbackManager::deregisterCallback(cb, WASABI_API_COMPONENT->getThisComponent());
#else
  CallbackManager::deregisterCallback(cb, NULL);
#endif
  return 1;
}

int api_syscbI::syscb_issueCallback(int eventtype, int msg, int p1, int p2) {
  CallbackManager::issueCallback(eventtype, msg, p1, p2);
  return 1;
}

