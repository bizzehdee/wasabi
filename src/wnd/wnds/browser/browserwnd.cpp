#include "precomp.h"

#include "browserwnd.h"

//FUCKO #include "mbsvc.h"

#include <bfc/attrib/cfgitem.h>

#if defined(_MSC_VER) && _MSC_VER < 1300
#include <atlimpl.cpp>
#endif //atlimpl.cpp is obsolite in newer versions of vc/psdk
#include <windows.h>
#include <mshtml.h>
#include <atldef.h>
#include <atliface.h>
#include <exdisp.h>
#include "atlhost.h"
extern "C" BOOL AtlMain(HINSTANCE hInstance);
extern "C" void AtlQuit(void);

namespace {//these are private helper version so we can avoid linking to other shit

STDAPI WriteBSTR(BSTR *pstrDest, LPCWSTR szSrc) {
  *pstrDest = SysAllocString( szSrc );
  if( !(*pstrDest) ) return E_OUTOFMEMORY;
  return NOERROR;
}

STDAPI FreeBSTR(BSTR* pstr) {
  if( *pstr == NULL ) return S_FALSE;
  SysFreeString( *pstr );
  return NOERROR;
}

STDAPI writeBString(BSTR* psz, const char *str) {
  WCHAR WideStr[WA_MAX_PATH];
  String s = str;
  if (s.isempty()) s = "";
  MultiByteToWideChar(CP_ACP, 0, s, -1, WideStr, WA_MAX_PATH);
  return WriteBSTR(psz, WideStr);
}

}
#ifdef MULTIPLE_BROWSERWND
BOOL BrowserWnd::oleOk = FALSE;
BOOL BrowserWnd::atlInitialized = FALSE;
#endif
IInternetZoneManager * BrowserWnd::pZoneHelper = NULL;

BrowserWnd::BrowserWnd(const char *initial_url, int browsertype) {
  if (initial_url == NULL) initial_url = "about:blank";

  browser_type = browsertype;
  setVirtual(0);
  iwebbrowser = NULL;
  ihostwnd = NULL;
  iunk = NULL;
  contain = NULL;
#ifndef MULTIPLE_BROWSERWND
  oleOk = FALSE;
  atlInitialized = FALSE;
#endif
  homepage = initial_url;
  timerset1 = 0;
  timerset2 = 0;
  silent = FALSE;
  accept_dnd = -1;
  scrollbarsflag = BROWSER_SCROLLBARS_DEFAULT;
  m_decorator = NULL;
  mainBrowser = NULL;
  m_clientsite = NULL;
  m_defaultclientsite = NULL;
  m_defaultcommandtarget = NULL;
  noinitnav = 0;
  noinitconnect = 0;
  m_events_connected = 0;
}

BrowserWnd::~BrowserWnd() {
  if (timerset1) {
    killTimer(MB_TIMERID1);
    timerset1 = 0;
  }
  if (timerset2) {
    killTimer(MB_TIMERID2);
    timerset2 = 0;
  }
  freeBrowserStuff();
}

int BrowserWnd::onInit() {
  BROWSER_PARENT::onInit();
  if (isVisible())
    onSetVisible(1);
  updateScrollbars();
  return 1;
}

void BrowserWnd::onSetVisible(int show) {
  if (show) initBrowserStuff();
  BROWSER_PARENT::onSetVisible(show);
#ifdef WIN32
  ShowWindow(getOsWindowHandle(), show ? SW_NORMAL : SW_HIDE);
#endif
}

int BrowserWnd::initBrowserStuff() {
DebugString("BrowserWnd::initBrowserStuff");
  if (iwebbrowser) return 1;
#ifndef MULTIPLE_BROWSERWND
  if (!atlInitialized) {
#ifdef WASABINOMAINAPI
    AtlMain(WASABI_API_APP->main_gethInstance());
#else
    AtlMain(wacmb->gethInstance());
#endif
    atlInitialized=TRUE;
  }

  if (SUCCEEDED(OleInitialize(NULL))) 
    oleOk = TRUE;
  if (!oleOk) return 1;
#endif
// {280876CF-48C0-40bc-8E86-73CE6BB462E5}
const GUID options_guid = 
{ 0x280876cf, 0x48c0, 0x40bc, { 0x8e, 0x86, 0x73, 0xce, 0x6b, 0xb4, 0x62, 0xe5 } };
  HWND hwndControl=gethWnd();

  int usemozilla = (browser_type == WBROWSER_MOZ);

//#ifdef WASABI_COMPILE_CONFIG
//  usemozilla = _intVal(WASABI_API_CONFIG->config_getCfgItemByGuid(options_guid), "Use Mozilla instead of IE for minibrowser");
//#endif

  int r = 0;
#if 1
//ASSERT(usemozilla);
//  if (usemozilla) {
    // try mozilla first
    r = (SUCCEEDED(ATL::AtlAxCreateControlEx(
      L"{1339B54C-3453-11D2-93B9-000000000000}",
      hwndControl,NULL,&contain,&iunk,IID_IWebBrowser2,NULL)) && iunk && contain);
//  }
#endif

#if 0
  
  CComObject<CAxHostWindow> * test;
  CComObject<CAxHostWindow>::CreateInstance(&test);
  test->SetAmbientDispatch((IDispatch *)m_clientsite);
  test->QueryInterface(&contain);
  if (!r) r = (SUCCEEDED(test->CreateControlEx(L"Shell.Explorer",hwndControl,NULL,&iunk,IID_IWebBrowser2,NULL)));
#endif
  // fall back on IE
  if (!r) r = (SUCCEEDED(ATL::AtlAxCreateControlEx(
    L"Shell.Explorer",
    hwndControl,NULL,&contain,&iunk,IID_IWebBrowser2,NULL)) && iunk && contain);
  //if( !r) r = SUCCEEDED(CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IUnknown, reinterpret_cast<void **>(&iunk)));


  if (r) {
    iunk->QueryInterface(IID_IWebBrowser2,(void**)&iwebbrowser);
    iunk->QueryInterface(IID_IOleCommandTarget, (void **)&m_defaultcommandtarget);
    if (iwebbrowser) {
      iwebbrowser->QueryInterface(IID_IDispatch,(void**)&mainBrowser);
      iwebbrowser->put_RegisterAsBrowser(VARIANT_TRUE);
      iwebbrowser->put_Silent(silent);
      if (accept_dnd != -1) iwebbrowser->put_RegisterAsDropTarget(accept_dnd);
      if ( !noinitconnect ) 
        connect();
      if ( m_clientsite )  {
        HRESULT hr;
        IOleObject* pOleObj;
        IOleClientSite * clientsite = NULL;
        if (SUCCEEDED(hr = m_clientsite->QueryInterface(IID_IOleClientSite, (void**)&clientsite)) && clientsite) {
          if (SUCCEEDED(hr = iwebbrowser->QueryInterface(IID_IOleObject, (void**)&pOleObj)) && pOleObj) {
            pOleObj->GetClientSite(&m_defaultclientsite);
            pOleObj->SetClientSite(clientsite);
            pOleObj->Release();
          }
          clientsite->Release();
        }
      }
      if ( ! noinitnav ) {
      if (deferednavigate.isempty()) {
        if(!homepage.isempty())
          minibrowser_navigateUrl(homepage);
        else
          minibrowser_navigateUrl("about:blank");
      } else 
        minibrowser_navigateUrl(deferednavigate);
      }
      ihostwnd=NULL;
      if( contain ) contain->QueryInterface(IID_IAxWinHostWindow,(void**)&ihostwnd);

      if (ihostwnd) {
        if ( !m_clientsite && m_decorator )
          ihostwnd->SetExternalUIHandler(m_decorator);
      }
    }
  }
//FUCKO should be a settable option  ShowWindow(hwndControl,SW_SHOWNA);
  return 1;
}


void BrowserWnd::connect() {
  if (iwebbrowser && !m_events_connected ) {
    AtlGetObjectSourceInterface(iwebbrowser, &m_libid, &m_iid, &m_wMajorVerNum, &m_wMinorVerNum);
    if ( DispEventAdvise(iwebbrowser, &m_iid) == S_OK )
      m_events_connected = 1;
  }
}

void BrowserWnd::disconnect() {
  if (iwebbrowser && m_events_connected ) {
    DispEventUnadvise(iwebbrowser, &m_iid);
    m_events_connected = 0;
  }
}

void BrowserWnd::freeBrowserStuff() {
  if (oleOk) {
    disconnect();
    
    if (ihostwnd) {
      ihostwnd->Release();
      ihostwnd=NULL;
    }
    if (iunk) {
      iunk->Release();
      iunk=NULL;
    }
    if (contain) {
      contain->Release();
      contain=NULL;
    }
    if (mainBrowser) {
      mainBrowser->Release();
      mainBrowser=NULL;
    }
    if (m_defaultcommandtarget) {
      m_defaultcommandtarget->Release();
      m_defaultcommandtarget=NULL;
    }
    if (m_defaultclientsite) {
      m_defaultclientsite->Release();
      m_defaultclientsite=NULL;
    }

    if (iwebbrowser) {
      iwebbrowser->Release();
      iwebbrowser=NULL;
    }
#ifndef MULTIPLE_BROWSERWND
    if (atlInitialized) {
      AtlQuit();
      atlInitialized = FALSE;
    }
    OleUninitialize();
    oleOk = FALSE;
#endif
#ifndef WASABINOMAINAPI
    api->hint_garbageCollect();
#endif
  }
}

int BrowserWnd::minibrowser_navigateUrl(const char *url) {
  if (url == NULL || *url == '\0') return 0;
DebugString("browser navigate: %s", url);

  curpage = url;

  if (!iwebbrowser) {
    deferednavigate = url;
    return 0;
  }

  MemBlock<wchar_t> wcstr(STRLEN(url)+1);
  mbstowcs(wcstr.m(), url, wcstr.getSizeInBytes());

//CUT DebugString("NAVIGATE AND SHIT");
  int r = iwebbrowser->Navigate(wcstr, NULL, NULL, NULL, NULL);
//CUT DebugString("nav result: %d", r);
  return 1;
}

int BrowserWnd::minibrowser_back() {
  ASSERT(iwebbrowser != NULL);
  iwebbrowser->GoBack();
  return 1;
}

int BrowserWnd::minibrowser_forward() {
  ASSERT(iwebbrowser != NULL);
  iwebbrowser->GoForward();
  return 1;
}

int BrowserWnd::minibrowser_refresh(int full) {
  ASSERT(iwebbrowser != NULL);
  if (!full)
    iwebbrowser->Refresh();
//  else {
//    VARIANT v = REFRESH_COMPLETELY;
//    iwebbrowser->Refresh2(&v);
//  }
  return 1;
}

int BrowserWnd::minibrowser_home() {
  ASSERT(iwebbrowser != NULL);
  minibrowser_navigateUrl(homepage);
  return 1;
}

int BrowserWnd::minibrowser_stop() {
  ASSERT(iwebbrowser != NULL);
  iwebbrowser->Stop();
  return 1;
}

HWND BrowserWnd::getOSHandle() {
  return GetWindow(gethWnd(), GW_CHILD); // assumes setVirtual(0) in constructor
}


void BrowserWnd::onTargetNameTimer() {
  updateTargetName();
}

void BrowserWnd::onScrollbarsFlagTimer() {
  updateScrollbars();
}

void BrowserWnd::timerclient_timerCallback(int id) {
  switch (id) {
    case MB_TIMERID1:
      onTargetNameTimer();
      return;
    case MB_TIMERID2: 
      onScrollbarsFlagTimer();
      return;
  }
  BROWSER_PARENT::timerclient_timerCallback(id);
}

void BrowserWnd::minibrowser_setTargetName(const char *name) {
  targetname = name;
  updateTargetName();
}

void BrowserWnd::updateTargetName() {
  if (!doSetTargetName(targetname)) {
    if (!timerset1) { timerclient_setTimer(MB_TIMERID1, 100); timerset1 = 1; }
    return;
  } else {
    if (timerset1) { timerclient_killTimer(MB_TIMERID1); timerset1 = 0; }
  }
}

int BrowserWnd::doSetTargetName(const char *name) {
  if (!iwebbrowser) return 0;
  IDispatch *id;
  if (SUCCEEDED(iwebbrowser->get_Document(&id)) && id) {
    IHTMLDocument2 *doc;
    if (SUCCEEDED(id->QueryInterface(IID_IHTMLDocument2, (void **)&doc)) && doc) {
      IHTMLWindow2 *w;
      if (SUCCEEDED(doc->get_parentWindow(&w)) && w) {
        
        wchar_t wcstr[WA_MAX_PATH];
        mbstowcs(wcstr, targetname, sizeof(wcstr));
        w->put_name(wcstr);
        w->Release();
        doc->Release();
        id->Release();
        return 1;
      }
    doc->Release();
    }
    id->Release();
  }
  return 0;
}

const char *BrowserWnd::minibrowser_getTargetName() {
  return targetname;
}

void BrowserWnd::event_onBeforeNavigate(const char *url, int flags, const char *targetframename, int *cancelnavigation) {
  int i=0;
  foreach(callbacks)
    int r = callbacks.getfor()->minibrowsercb_onBeforeNavigate(url, flags, targetframename);
    if (i++ == 0) *cancelnavigation = r;
  endfor;
  updateScrollbars();
  if ( !*cancelnavigation )
    resetFrames();
}

void BrowserWnd::minibrowser_setScrollbarsFlag(int a) {
  scrollbarsflag = a;
  updateScrollbars();
}

void BrowserWnd::updateScrollbars() {
  if (!doSetScrollbars()) {
    if (!timerset2) { timerclient_setTimer(MB_TIMERID2, 100); timerset2 = 1; }
    return;
  } else {
    if (timerset2) { timerclient_killTimer(MB_TIMERID2); timerset2 = 0; }
  }
}

void BrowserWnd::event_onDocumentComplete(const char *url) {
  curpage = url;
  if (!targetname.isempty())
    minibrowser_setTargetName(targetname);
  foreach(callbacks)
    callbacks.getfor()->minibrowsercb_onDocumentComplete(url);
  endfor;
  updateScrollbars();
}

int BrowserWnd::doSetScrollbars() {
  if (!iwebbrowser) return 0;
  if (scrollbarsflag == BROWSER_SCROLLBARS_DEFAULT) return 1;
  IDispatch *id;
  if (SUCCEEDED(iwebbrowser->get_Document(&id)) && id) {
    IHTMLDocument2 *doc;
    if (SUCCEEDED(id->QueryInterface(IID_IHTMLDocument2, (void **)&doc)) && doc) {
      IHTMLElement *e;
      if (SUCCEEDED(doc->get_body(&e))) {
        IHTMLStyle *s;
        if (SUCCEEDED(e->get_style(&s))) {
          BSTR a;
          switch (scrollbarsflag) {
            case BROWSER_SCROLLBARS_ALWAYS:
              writeBString(&a, "scroll");
              break;
            case BROWSER_SCROLLBARS_AUTO:
              writeBString(&a, "auto");
              break;
            case BROWSER_SCROLLBARS_NEVER:
              writeBString(&a, "hidden");
              break;
          }
          s->put_overflow(a);
          FreeBSTR(&a);
          s->Release();
          return 1;
        }
        e->Release();
      }
      doc->Release();
    }
    id->Release();
  }
  return 0;
}

const char *BrowserWnd::minibrowser_getCurrentUrl() {
  String ret;
  BSTR bstr;

  if (iwebbrowser == NULL) return "{error: browser not initialized}";
  ASSERT(iwebbrowser != NULL);

  int r = iwebbrowser->get_LocationURL(&bstr);
  if (r != S_OK) return String("");

  char txt[WA_MAX_PATH]="";
  wcstombs(txt, (const wchar_t *)bstr, WA_MAX_PATH-1);
  txt[WA_MAX_PATH-1] = 0;
  ret = txt;

  SysFreeString(bstr);

  curpage = ret;

  return curpage;
}

String BrowserWnd::browser_getCurrentTitle() {
  if (iwebbrowser == NULL) return "{error: browser not initialized}";
  ASSERT(iwebbrowser != NULL);

  String ret;
  BSTR bstr;

  int r = iwebbrowser->get_LocationName(&bstr);
  if (r != S_OK) return String("");

  char txt[WA_MAX_PATH]="";
  wcstombs(txt, (const wchar_t *)bstr, WA_MAX_PATH-1);
  txt[WA_MAX_PATH-1] = 0;
  ret = txt;

  return ret;
}

String BrowserWnd::browser_getCurrentStatusText() {	// doesn't work
  if (iwebbrowser == NULL) return "{error: browser not initialized}";
  ASSERT(iwebbrowser != NULL);

  String ret;
  BSTR bstr;

  int r = iwebbrowser->get_StatusText(&bstr);
  if (r != S_OK) return StringPrintf("*error* %x", r);

  char txt[WA_MAX_PATH]="";
  wcstombs(txt, (const wchar_t *)bstr, WA_MAX_PATH-1);
  txt[WA_MAX_PATH-1] = 0;
  ret = txt;

  SysFreeString(bstr);

  return ret;
}

void BrowserWnd::browser_setSilent(int v) {
  silent = v;
  if (iwebbrowser != NULL) iwebbrowser->put_Silent(v);
}

void BrowserWnd::browser_setAcceptDND(int v) {
  accept_dnd = v;
  if (iwebbrowser != NULL) iwebbrowser->put_RegisterAsDropTarget(v);
}

void BrowserWnd::browser_setTextZoom(int v) {
  if (!iwebbrowser) return;
  IDispatch *id=NULL;
  if (SUCCEEDED(iwebbrowser->get_Document(&id)) && id) {
    LPOLECOMMANDTARGET targ=NULL;
    if (SUCCEEDED(id->QueryInterface(IID_IOleCommandTarget, (void **)&targ)) && targ) {
      VARIANT vaZoomFactor;
      VariantInit(&vaZoomFactor);
      V_VT(&vaZoomFactor) = VT_I4;
      V_I4(&vaZoomFactor) = MINMAX(v, 0, 5);
      targ->Exec(NULL, OLECMDID_ZOOM, OLECMDEXECOPT_DONTPROMPTUSER,
                 &vaZoomFactor, NULL);
      VariantClear(&vaZoomFactor);
      targ->Release(); // release document's command target
    }
    id->Release();    // release document's dispatch interface
  }
}

HRESULT BrowserSinkObj::OnBeforeNavigate2(LPDISPATCH pDisp, VARIANT* _url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel) {

  char url[WA_MAX_PATH]="";
  wcstombs(url, _url->bstrVal, WA_MAX_PATH-1);
  url[WA_MAX_PATH-1]=0;

  int flags = Flags->intVal;

  char target[256]="";
  if (TargetFrameName->bstrVal != NULL) {
    wcstombs(target, TargetFrameName->bstrVal, 255);
    target[255]=0;
  }

  int cancel=0;
  if ( pDisp == getMainBrowserDisp() ) {
    event_onBeforeNavigate(url, flags, target, &cancel);
  }
  else {
    int idx = getFrameDispIdx( (void *)pDisp);
    event_onFrameBeforeNavigate(url, flags, target, &cancel,idx);
  }
  *Cancel = cancel ? -1 : 0;
  return S_OK;
}

HRESULT BrowserSinkObj::OnDocumentComplete(IDispatch *pDisp, VARIANT *_url) {
  char url[WA_MAX_PATH]="";
  if ( _url->bstrVal )
    wcstombs(url, _url->bstrVal, WA_MAX_PATH-1);
  url[WA_MAX_PATH-1]=0;
  if ( pDisp == getMainBrowserDisp() ) 
    event_onDocumentComplete(url);
  else {
    int idx = getFrameDispIdx( (void *)pDisp);
    event_onFrameDocumentComplete(url, idx);

  }
  return S_OK;
}

HRESULT BrowserSinkObj::OnNavigateComplete(IDispatch *pDisp, VARIANT *_url) {
  char url[WA_MAX_PATH]="";
  if ( _url->bstrVal )
    wcstombs(url, _url->bstrVal, WA_MAX_PATH-1);
  url[WA_MAX_PATH-1]=0;
  if ( pDisp == getMainBrowserDisp() ) 
    event_onNavigateComplete(url);
  else {
    int idx = getFrameDispIdx( (void *)pDisp);
    event_onFrameNavigateComplete(url, idx);
  }
  return S_OK;
}

HRESULT BrowserSinkObj::OnStatusTextChange(BSTR text) {
  if (text == NULL) {
    event_onStatusTextChange("");
  } else {
    char txt[WA_MAX_PATH]="";
    wcstombs(txt, text, WA_MAX_PATH-1);
    txt[WA_MAX_PATH-1] = 0;
    if (*txt) {
      event_onStatusTextChange(txt);
    }
    else
      event_onStatusTextChange("");
  }
  return (HRESULT)S_OK;
}

HRESULT BrowserSinkObj::OnTitleChange(BSTR text) {
  if (text == NULL) {
    event_onTitleChange("");
  } else {
    char txt[WA_MAX_PATH]="";
    wcstombs(txt, text, WA_MAX_PATH-1);
    txt[WA_MAX_PATH-1] = 0;
    if (*txt) {
      event_onTitleChange(txt);
    }
  }
  return (HRESULT)S_OK;
}

HRESULT BrowserSinkObj::OnProgressChange(long nProgress,long nProgressMax)  {
  event_onProgressChange(nProgress,nProgressMax);
  return (HRESULT)S_OK;
}


HRESULT BrowserSinkObj::OnNewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel)  {
  int cancel = 0;
  void * newdisp = NULL;
  event_onNewWindow(&newdisp,&cancel);
  if ( cancel && Cancel)
    *Cancel = VARIANT_TRUE;
  if ( newdisp && ppDisp )
    *ppDisp = (IDispatch *) newdisp;
  return (HRESULT)S_OK;
}

HRESULT BrowserSinkObj::OnNavigateError(IDispatch *pDisp, VARIANT *_url, VARIANT *TargetFrameName, VARIANT *StatusCode, BOOL *Cancel) {

  char url[WA_MAX_PATH]="";
  wcstombs(url, _url->bstrVal, WA_MAX_PATH-1);
  url[WA_MAX_PATH-1]=0;

  int status = StatusCode->intVal;

  char target[256]="";
  if (TargetFrameName->bstrVal != NULL) {
    wcstombs(target, TargetFrameName->bstrVal, 255);
    target[255]=0;
  }

  int cancel=0;
  if ( pDisp == getMainBrowserDisp() ) {
    event_onNavigateError(url, status, target, &cancel);
  }
  else {
    int idx = getFrameDispIdx( (void *)pDisp);
    event_onFrameNavigateError(url, status, target, &cancel,idx);
  }
  if (Cancel)
    *Cancel = cancel ? -1 : 0;
  return S_OK;
}

HRESULT BrowserSinkObj::OnSetSecureLockIcon(long SecureLockIcon)  {  
  event_OnSetSecureLockIcon(SecureLockIcon);
  return S_OK;
}

void BrowserWnd::event_onFrameBeforeNavigate(const char *url, int flags, const char *targetframename, int *cancelnavigation, int idx)  {
  event_onBeforeNavigate(url,flags,targetframename,cancelnavigation);
}

void BrowserWnd::event_onFrameDocumentComplete(const char *url, int idx)  {
  event_onDocumentComplete(url);
}

void BrowserWnd::event_onFrameNavigateError(const char *url, int status, const char *targetframename, int *cancel, int idx)  { 
  event_onNavigateError(url, status, targetframename, cancel);
}

int BrowserWnd::getFrameDispIdx( void * disp )  {
  IDispatch * item = (IDispatch *) disp;
  int res = frames.searchItem(item);
  if ( res != -1 )
    return res;
  res = frames.getNumItems();
  frames.addItem(item);
  return res;
}

void BrowserWnd::removeSite() {
  if (iwebbrowser) {
    if ( m_clientsite )  {
      HRESULT hr;
      IOleObject* pOleObj;
      if (SUCCEEDED(hr = iwebbrowser->QueryInterface(IID_IOleObject, (void**)&pOleObj)) && pOleObj) {
        pOleObj->SetClientSite(m_defaultclientsite);
        pOleObj->Release();
      }
    }
  }
}

#ifdef MULTIPLE_BROWSERWND
int BrowserWnd::Initialize()  {
  if (!atlInitialized) {
#ifdef WASABINOMAINAPI
    AtlMain(WASABI_API_APP->main_gethInstance());
#else
    AtlMain(wacmb->gethInstance());
#endif
    atlInitialized=TRUE;
  }

  if (SUCCEEDED(OleInitialize(NULL))) 
    oleOk = TRUE;
  if (!oleOk) return 1;
  if ( !SUCCEEDED(CoCreateInstance(CLSID_InternetZoneManager,NULL, CLSCTX_INPROC_SERVER, IID_IInternetZoneManager, (void **)&pZoneHelper) ))
    pZoneHelper = NULL;
  return 0;
}


void BrowserWnd::Shutdown()  {
    if (atlInitialized) {
      AtlQuit();
      atlInitialized = FALSE;
    }
    if ( pZoneHelper ) 
      pZoneHelper->Release();
    OleUninitialize();
    oleOk = FALSE;
}

#endif

void *BrowserWnd::GetZoneHelper() {
  return pZoneHelper;
}
