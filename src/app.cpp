//
// This is only an example of a Wasabi app that creates a basic window.
// If you want to build your app from here, do not use this file in
// place. Copy it to your project directory and modify it there.
// Then add it to your project along with main.cpp.
//

#include "precomp.h"

#include <wnd/wnds/blankwnd.h>

#ifdef WASABI_PLATFORM_WIN32
#include "resource.h"
#endif

#include "app.h"

//EDITME: replace with your own app info
#define APPNAME "SuperDuperApp"
#define STRFILEVER "1, 0, 0, 1\0"	// autobuild.dll format

const char *appname = APPNAME;
const char *strfilever = STRFILEVER;

//EDITME: replace with your own app guid
// {10C105C9-7904-4b4f-B0D4-74D6D0FB4F2D}
static const GUID appguid = 
{ 0x14444449, 0x7404, 0x4b44, { 0x40, 0xd4, 0x44, 0xd6, 0xd4, 0xf4, 0x4f, 0x2d } };
const GUID *pappguid = &appguid;

extern OSMODULEHANDLE wasabiAppOSModuleHandle; // WinMain fills in

void wasabiAppInit(OSWINDOWHANDLE parentwnd, const char *cmdline) {
  // wee
  BlankWnd *t = new BlankWnd;
  t->setVirtual(0);
  t->init(wasabiAppOSModuleHandle, parentwnd);
}

void wasabiAppShutdown() {
  // la
}
