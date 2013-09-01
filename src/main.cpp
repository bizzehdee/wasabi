#include <precomp.h>
#include <api/apiinit.h>
#include <api/service/svcmgr.h>
#include <api/application/version.h>


#include "app.h"

DECLARE_MODULE_SVCMGR;

extern const char *appname;
extern const char *strfilever;
extern const GUID *pappguid;	// filled in by app.cpp

OSMODULEHANDLE wasabiAppOSModuleHandle; // WinMain fills in

extern TList<OSWINDOWHANDLE> dlglist;
int exitpump = 0;

void doMessagePump() {
  exitpump = 0;
  // Despite appearances, this is portable
  MSG msg;
  while (!exitpump && GetMessage( &msg, INVALIDOSWINDOWHANDLE, 0, 0 ) ) {
#ifdef WIN32
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x20A
#endif
    // umm... win32 click-to-mousewheel sucks. let's suck less. -BU
    if (msg.message == WM_MOUSEWHEEL) {
      HWND h = WindowFromPoint(msg.pt);
      SendMessage(h, WM_MOUSEWHEEL, msg.wParam, msg.lParam);// NOT Post ;)
      continue;
    }
    int isd=0;
    for (int i = 0; i < dlglist.getNumItems(); i++) {
      if (IsDialogMessage(dlglist[i], &msg)) {
        isd = 1;
        break;
      }
    }
    if (!isd)
#endif
    {
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
  }
}

int APIENTRY WinMain(OSMODULEHANDLE hInstance,
		     OSMODULEHANDLE hPrevInstance,
		     LPSTR     lpCmdLine,
		     int       nCmdShow) {

  ASSERTPR(appname != NULL, "need to include app.cpp and fill out appname");
  ASSERTPR(pappguid != NULL, "need to include app.cpp and fill out appguid");
  ASSERTPR(*pappguid != INVALID_GUID, "need to include app.cpp and fill out appguid");
  wasabiAppOSModuleHandle = hInstance;

  WasabiVersion::setAppName(appname);
  WasabiVersion::setBuildNumber(WasabiVersion::versionStringToInt(strfilever));

#ifdef WASABI_COMPILE_WND
  // the windowing API will automatically create a toplevel wnd for you
  OSWINDOWHANDLE parentwnd = WASABI_API_WND->wnd_getMainWnd()
#else
  // not using windowing API
  // create a master parent wnd here if you need/want one
  OSWINDOWHANDLE parentwnd = INVALIDOSWINDOWHANDLE;
#endif

  ApiInit::init(hInstance, *pappguid, "", parentwnd);

  // make windows and things
  wasabiAppInit(parentwnd, lpCmdLine);

  doMessagePump();

  wasabiAppShutdown();

  ApiInit::shutdown();

DebugString("shutdown");

  return 0;
}

#ifdef WASABI_PLATFORM_POSIX
int main( int argc, char *argv[] ) {
  OSMODULEHANDLE hInstance = dlopen( NULL, RTLD_NOW );
  int len = 0;

  for( int i = 1; i < argc; i++ ) {
    len += strlen( argv[i] );
  }
  if ( argc > 2 )
    len += argc - 2; // Spaces
  len += 2 * (argc - 1);

  char *lpCmdLine = (char *)malloc( len + 1 );

  *lpCmdLine = 0;

  for ( int i = 1; i < argc; i++ ) {
    if ( i > 1 )
      strcat( lpCmdLine, " " );
  
    strcat( lpCmdLine, "\"" );
    strcat( lpCmdLine, argv[i] );  
    strcat( lpCmdLine, "\"" );
  }

  WinMain( hInstance, NULL, lpCmdLine, 0 );

  free( lpCmdLine );
}
#endif

