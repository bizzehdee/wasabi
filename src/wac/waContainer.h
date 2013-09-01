#ifndef __WASABI_WACONTAINER_H_
#define __WASABI_WACONTAINER_H_

#include <precomp.h>
#include <bfc/loadlib.h>
#include <wac/wac.h>

class waContainer {
public:
	waContainer(const char *filename);
	virtual ~waContainer();
	void callCreate();
	void callDestroy();

	WaComponent *getWaComponent();

  const char *getName();
  const GUID getGuid();

  const char *getFilename() { return wacLib.getName(); }

private:
	Library wacLib;
	WACINIT initProc;
	WACGETCOMPONENT getWacProc;
	WACGETVERSION versionProc;
	WaComponent *wacomp;
};

#endif // __WASABI_WACONTAINER_H_
