#ifndef __WASABI_CFG_H
#define __WASABI_CFG_H

#ifdef __cplusplus

#define _WASABIRUNTIME

// Uncomment this to have an old-style global api pointer
#define WA3COMPATIBILITY


#ifndef _WASABIRUNTIME

#ifndef WA3COMPATIBILITY
#define WASABINOMAINAPI
#endif

/* 

Comment or uncomment the following directives according to the needs of your application :

*/

/* note to the team:

  the WANT_WASABI_API_* directives will go away once we're done splitting the api, their only purpose
  is to split the api one bit at a time while the rest remains working. when it's done, all that will remain
  will be the WASABI_COMPILE_* directives

*/


// This allows component (external plugins)
//#define WASABI_COMPILE_COMPONENTS

// This enables the layered UI
//#define WASABI_COMPILE_WND

// This enables multiplexed timers
//#define WASABI_COMPILE_TIMERS

// This enables xml group loading within the window api
//#define WASABI_COMPILE_SKIN

// This enables internationalizaiton
// #define WASABI_COMPILE_UTF

// This enables action handling in UI objects (clicks and keypresses)
//#define WASABI_COMPILE_ACTIONS

// This enables UI scripting
//#define WASABI_COMPILE_SCRIPT

// This enables keyboard locales in UI
//#define WASABI_COMPILE_LOCALES

// This enables bitmap and truetype font rendering
//#define WASABI_COMPILE_FONTS

// This sets the static font renderer. If you are compiling with api_config, the attribute to set is { 0x280876cf, 0x48c0, 0x40bc, { 0x8e, 0x86, 0x73, 0xce, 0x6b, 0xb4, 0x62, 0xe5 } }, "Font Renderer"
//#define WASABI_FONT_RENDERER "" // "" is Win32

// This lets you override all bitmapfonts using TTF fonts (for internationalization). define to a function call or a global value to change this value dynamically. 
// If you are compiling with api_config, the attribute to set is { 0x280876cf, 0x48c0, 0x40bc, { 0x8e, 0x86, 0x73, 0xce, 0x6b, 0xb4, 0x62, 0xe5 } }, "Use bitmap fonts (no international support)"
//#define WASABI_FONT_TTFOVERRIDE 0 // 1 does all rendering with TTF
                        
// This enables loading for pngs, jpgs (you need to add the necessary image loader services)
//#define WASABI_COMPILE_IMGLDR
//#define WASABI_COMPILE_IMGLDR_PNGREAD

// This enables metadb support
//#define WASABI_COMPILE_METADB

// This enables config file support
//#define WASABI_COMPILE_CONFIG

// This enables the filereader pipeline
//#define WASABI_COMPILE_FILEREADER

// This enables the xml parser for config and group loading
#define WASABI_COMPILE_XMLPARSER

// This enables system callback management
#define WASABI_COMPILE_SYSCB

// This enables centralized memory allocation/deallocation
//#define WASABI_COMPILE_MEMMGR

//#define WASABI_SKINS_SUBDIRECTORY "skins"

//#define WASABI_COMPILE_MEDIACORE

#define WASABI_COMPILE_MAKIDEBUG

#else // not _WASABIRUNTIME

// this section should define the entire set of wasabi parts since this is a full runtime build

#define WASABI_COMPILE_COMPONENTS
#define WASABI_COMPILE_SKIN
#define WASABI_COMPILE_ACTIONS
#define WASABI_COMPILE_SCRIPT
#define WASABI_COMPILE_FONTS
#define WASABI_COMPILE_LOCALES
#define WASABI_COMPILE_IMGLDR
#define WASABI_COMPILE_IMGLDR_PNGREAD
#define WASABI_COMPILE_METADB
#define WASABI_COMPILE_CONFIG
#define WASABI_COMPILE_FILEREADER
#define WASABI_COMPILE_XMLPARSER
#define WASABI_COMPILE_SYSCB
#define WASABI_COMPILE_MEMMGR
#define WASABI_COMPILE_SKIN_WA2
#define WASABI_COMPILE_PAINTSETS
#define WASABI_COMPILE_WNDMGR
#define WASABI_SKINS_SUBDIRECTORY "skins"
#define WASABI_RESOURCES_SUBDIRECTORY "wacs"
#define WASABI_FONT_RENDERER "" // "" is Win32
#define WASABI_COMPILE_MEDIACORE
#define WASABI_COMPILE_TIMERS
#define WASABI_COMPILE_WND
#define WASABI_COMPILE_UTF   
#define WASABI_WNDMGR_ALLCONTAINERSDYNAMIC 0
#define WASABI_COMPILE_MAKIDEBUG

#endif // not _WASABIRUNTIME

#ifdef _WASABIRUNTIME
  #define WASABI_API_SYSTEM      api
  #define WASABI_API_APP         api
  #define WASABI_API_COMPONENT   api
  #define WASABI_API_SVC         api
  #define WASABI_API_SYSCB       api
  #define WASABI_API_MAKI        api
  #define WASABI_API_UTF         api
  #define WASABI_API_WND         api
  #define WASABI_API_IMGLDR      api
  #define WASABI_API_FILE        api
  #define WASABI_API_TIMER       api
  #define WASABI_API_WNDMGR      api
  #define WASABI_API_SKIN        api
  #define WASABI_API_METADB      api
  #define WASABI_API_LOCALE      api
  #define WASABI_API_CONFIG      api
  #define WASABI_API_FONT        api
  #define WASABI_API_MEMMGR      api
  #define WASABI_API_XML         api
  #define WASABI_API_MEDIACORE   api
  #define WASABI_API_MAKIDEBUG   debugApi

//  #define EXPERIMENTAL_INDEPENDENT_AOT

#else // _WASABIRUNTIME

  #define WASABI_API_SYSTEM      systemApi
  
  #define WASABI_API_APP         applicationApi
  #define WASABI_API_SVC         serviceApi
  #define WASABI_API_SYSCB       sysCallbackApi

  #ifdef WASABI_COMPILE_COMPONENTS
  #define WASABI_API_COMPONENT   componentApi
  #endif

  #ifdef WASABI_COMPILE_SCRIPT
  #define WASABI_API_MAKI        makiApi
  #endif

  #ifdef WASABI_COMPILE_UTF
  #define WASABI_API_UTF         utfApi
  #endif

  #ifdef WASABI_COMPILE_WND
  #define WASABI_API_WND         wndApi
  #endif

  #ifdef WASABI_COMPILE_IMGLDR
  #define WASABI_API_IMGLDR      imgLoaderApi
  #endif

  #ifdef WASABI_COMPILE_FILEREADER
  #define WASABI_API_FILE        fileApi
  #endif

  #ifdef WASABI_COMPILE_TIMERS
  #define WASABI_API_TIMER       timerApi
  #endif

  #ifdef WASABI_COMPILE_WNDMGR
  #define WASABI_API_WNDMGR      wndManagerApi
  #endif

  #ifdef WASABI_COMPILE_SKIN
  #define WASABI_API_SKIN        skinApi
  #endif

  #ifdef WASABI_COMPILE_METADB
  #define WASABI_API_METADB      metadbApi
  #endif

  #ifdef WASABI_COMPILE_LOCALES
  #define WASABI_API_LOCALE      localeApi
  #endif

  #ifdef WASABI_COMPILE_CONFIG
  #define WASABI_API_CONFIG      configApi
  #endif

  #ifdef WASABI_COMPILE_FONTS
  #define WASABI_API_FONT        fontApi
  #endif

  #ifdef WASABI_COMPILE_MEMMGR
  #define WASABI_API_MEMMGR      memmgrApi
  #endif

  #ifdef WASABI_COMPILE_XMLPARSER
  #define WASABI_API_XML         xmlApi
  #endif

  #ifdef WASABI_COMPILE_MEDIACORE
  #define WASABI_API_MEDIACORE   coreApi
  #endif

  #ifdef WASABI_COMPILE_MAKIDEBUG
  #define WASABI_API_MAKIDEBUG   debugApi
  #endif

#endif // _WASABIRUNTIME

#define WASABI_EXTERNAL_GUIOBJECTS

#define WASABI_WIDGETS_GUIOBJECT
#define WASABI_WIDGETS_LAYER
#define WASABI_WIDGETS_TEXT
#define WASABI_WIDGETS_BUTTON
#define WASABI_WIDGETS_TGBUTTON
#define WASABI_WIDGETS_ANIMLAYER
#define WASABI_WIDGETS_GROUPLIST
#define WASABI_WIDGETS_MOUSEREDIR
#define WASABI_WIDGETS_SLIDER
#define WASABI_WIDGETS_MEDIASLIDERS
#define WASABI_WIDGETS_MEDIAVIS
#define WASABI_WIDGETS_MEDIAEQCURVE
#define WASABI_WIDGETS_MEDIASTATUS
#define WASABI_WIDGETS_SVCWND
#define WASABI_WIDGETS_EDIT
#define WASABI_WIDGETS_TITLEBAR
#define WASABI_WIDGETS_COMPBUCK

#define WASABI_WANT_FF_POPUP

#ifndef WASABI_EXTERNAL_GUIOBJECTS

#define WASABI_WIDGETS_BROWSER
#define WASABI_WIDGETS_FRAME
#define WASABI_WIDGETS_GRID
#define WASABI_WIDGETS_QUERYDRAG
#define WASABI_WIDGETS_QUERYLIST
#define WASABI_WIDGETS_FILTERLIST
#define WASABI_WIDGETS_QUERYLINE
#define WASABI_WIDGETS_WNDHOLDER
#define WASABI_WIDGETS_LAYOUTSTATUS
#define WASABI_WIDGETS_TABSHEET
#define WASABI_WIDGETS_CHECKBOX
#define WASABI_WIDGETS_TITLEBOX
#define WASABI_WIDGETS_CUSTOMOBJECT
#define WASABI_WIDGETS_RADIOGROUP
#define WASABI_WIDGETS_LIST
#define WASABI_WIDGETS_TREE
#define WASABI_WIDGETS_DROPDOWNLIST
#define WASABI_WIDGETS_COMBOBOX
#define WASABI_WIDGETS_HISTORYEDITBOX
#define WASABI_WIDGETS_OBJECTDIRECTORY
#define WASABI_WIDGETS_RECTANGLE
#define WASABI_WIDGETS_PATHPICKER
#define WASABI_WIDGETS_GRADIENT
#define WASABI_WIDGETS_MENUBUTTON

#define WASABI_TOOLOBJECT_HIDEOBJECT
#define WASABI_TOOLOBJECT_SENDPARAMS
#define WASABI_TOOLOBJECT_ADDPARAMS

#endif // WASABI_EXTERNAL_GUIOBJECTS

#define WASABI_SCRIPTOBJECTS_POPUP
#define WASABI_SCRIPTOBJECTS_LIST
#define WASABI_SCRIPTOBJECTS_BITLIST
#define WASABI_SCRIPTOBJECTS_REGION
#define WASABI_SCRIPTOBJECTS_TIMER
#define WASABI_SCRIPTOBJECTS_MAP

#ifndef WASABI_EXTERNAL_GUIOBJECTS
#define WASABI_SCRIPTOBJECTS_WAC
#endif // WASABI_EXTERNAL_GUIOBJECTS

#define WASABI_DEFAULT_STDCONTAINER "resizable_status"
#define DEFAULT_CROSSFADE_ENABLED TRUE

#define DEFAULT_DESKTOPALPHA       FALSE
#define DEFAULT_LINKLAYOUTSCALE    TRUE
#define DEFAULT_LINKLAYOUTSALPHA   FALSE
#define DEFAULT_LINKALLALPHA       FALSE
#define DEFAULT_LINKALLRATIO       FALSE
#define DEFAULT_LINKEDALPHA        191
#define DEFAULT_AUTOOPACITYTIME    2000
#define DEFAULT_AUTOOPACITYFADEIN  250
#define DEFAULT_AUTOOPACITYFADEOUT 1000
#define DEFAULT_AUTOOPACITYTYPE    0
#define DEFAULT_EXTENDAUTOOPACITY  25
#define DEFAULT_USERATIOLOCKS      TRUE
#define DEFAULT_TIMERRESOLUTION    33
#define DEFAULT_TOOLTIPS           TRUE
#define DEFAULT_TEXTSPEED          1.0f/3.0f

#define UTF8 1

//#include <bfc/api/api_system.h>
#include <bfc/api/api_application.h>
#include <bfc/api/api_service.h>
#include <bfc/api/api_syscb.h>
#include <bfc/api/api_memmgr.h>
//#include <bfc/api/api_component.h>
#include <bfc/api/api_maki.h>
//#include <bfc/api/api_utf.h>
#include <bfc/api/api_font.h>
#include <bfc/api/api_wnd.h>
#include <bfc/api/api_imgldr.h>
//#include <bfc/api/api_file.h>
#include <bfc/api/api_timer.h>
//#include <bfc/api/api_wndmgr.h>
//#include <bfc/api/api_element.h>
//#include <bfc/api/api_metadb.h>
//#include <bfc/api/api_locale.h>
#include <bfc/api/api_config.h>
#include <bfc/api/api_xml.h>
#include <bfc/api/api_skin.h>
#include <bfc/api/api_makidebug.h>

#endif // __cplusplus

#endif
