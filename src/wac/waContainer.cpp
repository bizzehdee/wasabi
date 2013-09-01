#include <precomp.h>
#include <wac/waContainer.h>

#include <api/service/api_servicei.h>
#include <bfc/assert.h>
#include <bfc/string/string.h>

waContainer::waContainer(const char *filename) : wacLib(filename), wacomp(NULL) {
	//load our wac file
	wacLib.load();

	//gather our wac functions
  initProc = (WACINIT)wacLib.getProcAddress("WAC_init");
  getWacProc = (WACGETCOMPONENT)wacLib.getProcAddress("WAC_getWaComponent");
  versionProc = (WACGETVERSION)wacLib.getProcAddress("WAC_getVersion");
	
  if(initProc == NULL || getWacProc == NULL || versionProc == NULL) {
    DebugString("WAC Failed To Load (invalid WAC):\n%s\n",filename);
    return;
  }

	//check the wac version against the version suported by the exe
  if(versionProc() == WA_COMPONENT_VERSION) {
    //init the wac
    initProc(wacLib.getHandle());

    wacomp = getWacProc();

    //if something goes bad and we dont get a wacomponent.
    if(!wacomp) {
      DebugString("WAC_getWaComponent Failed:\n%s\n",filename);
      return;
		}

		//all is good, register the api
    wacomp->registerServices(WASABI_API_SVC);

	} else {
		//yep, we're boned.. this wac is old :)
    DebugString("WAC Failed To Load (version out of date):\n%s\n",filename);
	}
}

void waContainer::callCreate() {
	//call onCreate
	wacomp->onCreate();
}

void waContainer::callDestroy() {
	//call onDestroy
	wacomp->onDestroy();
}

WaComponent *waContainer::getWaComponent() {
	return wacomp;
}

waContainer::~waContainer() {
	//remove api pointer
	if(wacomp) wacomp->deregisterServices();

	// and to be nice to the environment
	wacLib.unload();
}

const char *waContainer::getName() {
	return getWaComponent()->getName();
}

const GUID waContainer::getGuid() {
  return getWaComponent()->getGUID();
}