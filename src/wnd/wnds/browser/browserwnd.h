#ifndef _BROWSER_H
#define _BROWSER_H

class BrowserWnd;

#define MULTIPLE_BROWSERWND

#define BROWSER_PARENT OSWnd
#define IDC_SINKOBJ 0x9871 // arbitrary unique id
#define MB_TIMERID1 0x1927
#define MB_TIMERID2 0x1928

template <class T> class PtrList;

#include <bfc/string/string.h>

#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <exdispid.h>
#include <mshtmhst.h>
#include <mshtmdid.h>

class BrowserSinkObj : public IDispEventImpl<IDC_SINKOBJ, BrowserSinkObj> {
  public:
    BEGIN_SINK_MAP(BrowserSinkObj)
      SINK_ENTRY(IDC_SINKOBJ, DISPID_BEFORENAVIGATE2, OnBeforeNavigate2)
      SINK_ENTRY(IDC_SINKOBJ, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete)
      SINK_ENTRY(IDC_SINKOBJ, DISPID_STATUSTEXTCHANGE, OnStatusTextChange)
      SINK_ENTRY(IDC_SINKOBJ, DISPID_TITLECHANGE, OnTitleChange)
      SINK_ENTRY(IDC_SINKOBJ, DISPID_PROGRESSCHANGE, OnProgressChange)
      SINK_ENTRY(IDC_SINKOBJ, DISPID_NEWWINDOW2, OnNewWindow2)
//      SINK_ENTRY(IDC_SINKOBJ, DISPID_NAVIGATEERROR, OnNavigateError)
//      SINK_ENTRY(IDC_SINKOBJ, DISPID_SETSECURELOCKICON, OnSetSecureLockIcon)
      SINK_ENTRY(IDC_SINKOBJ, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete)
    END_SINK_MAP()

    virtual void event_onBeforeNavigate(const char *url, int flags, const char *targetframename, int *cancel) { }
    virtual void event_onDocumentComplete(const char *url) { }
    virtual void event_onNavigateComplete(const char *url) { }
    virtual void event_onStatusTextChange(const char *text) { }
    virtual void event_onTitleChange(const char *text) { }
    virtual void event_onFrameBeforeNavigate(const char *url, int flags, const char *targetframename, int *cancelnavigation, int idx) { }
    virtual void event_onFrameDocumentComplete(const char *url, int idx) { }
    virtual void event_onFrameNavigateComplete(const char *url, int idx) { }
    virtual void event_onProgressChange(long nProgress,long nProgressMax) { }
    // override if you want to allow new windows spawned
    virtual void event_onNewWindow(void **disp, int * cancel) { }
    virtual void event_onNavigateError(const char *url, int status, const char *targetframename, int *cancel) { }
    virtual void event_onFrameNavigateError(const char *url, int status, const char *targetframename, int *cancel, int idx) { }
    virtual void event_OnSetSecureLockIcon(long type) { }

    virtual void * getMainBrowserDisp() { return NULL; }
    virtual int getFrameDispIdx(void * disp) { return -1;}

    HRESULT __stdcall OnDocumentComplete(IDispatch *pDisp, VARIANT *url);
    HRESULT __stdcall OnNavigateComplete(IDispatch *pDisp, VARIANT *url);
    HRESULT __stdcall OnBeforeNavigate2(LPDISPATCH ppDisp, VARIANT* url, VARIANT *Flags, VARIANT *TargetFrameName, VARIANT *PostData, VARIANT *Headers, BOOL *Cancel);
    HRESULT __stdcall OnStatusTextChange(BSTR text);
    HRESULT __stdcall OnTitleChange(BSTR text);
    HRESULT __stdcall OnProgressChange(long nProgress,long nProgressMax);
    HRESULT __stdcall OnNewWindow2(IDispatch **ppDisp, VARIANT_BOOL *Cancel);
    HRESULT __stdcall OnNavigateError(IDispatch *pDisp, VARIANT * url, VARIANT *TargetFrameName, VARIANT *StatusCode, BOOL *Cancel);
    HRESULT __stdcall OnSetSecureLockIcon(long SecureLockIcon);
};



#include "browserenum.h"
#include "oswnd.h"
#include "minibrowser.h"

//hmm, nothing uses this? struct IHTMLDocument2;
struct IWebBrowser2;

class BrowserWnd : public BROWSER_PARENT, public BrowserSinkObj, public MiniBrowserI {
public:
  BrowserWnd(const char *initial_url=NULL, int browsertype=DEFAULT_BROWSER);
  virtual ~BrowserWnd();

  // RootWnd
  virtual int onInit();
  virtual void onSetVisible(int show);
  virtual int handleDesktopAlpha() { return 0; }

  // OSWnd
  virtual HWND getOSHandle();

  // MiniBrowser
  virtual int minibrowser_navigateUrl(const char *url);
  virtual void minibrowser_setHome(const char *url) { homepage = url; }
  virtual int minibrowser_back();
  virtual int minibrowser_forward();
  virtual int minibrowser_home();
  virtual int minibrowser_refresh(int full=0);
  virtual int minibrowser_stop();
  virtual void minibrowser_setTargetName(const char *name);
  const char *minibrowser_getTargetName();
  const char *minibrowser_getCurrentUrl();
  virtual void minibrowser_addCB(MiniBrowserCallback *cb) { callbacks.addItem(cb); }
  virtual RootWnd *minibrowser_getRootWnd() { return this; }

  virtual void minibrowser_setScrollbarsFlag(int a); //BROWSER_SCROLLBARS_ALWAYS, BROWSER_SCROLLBARS_AUTO, BROWSER_SCROLLBARS_NEVER

  String browser_getCurrentTitle();
  String browser_getCurrentStatusText();	// doesn't work

  void browser_setSilent(int v);
  void browser_setAcceptDND(int v);
  void browser_setTextZoom(int v);

  void setEmbeddedBrowserGuid(const char *guid);

  //
  virtual void timerclient_timerCallback(int id);

  void onTargetNameTimer();
  virtual void *getBrowser() { return  (void *) iwebbrowser; }
  virtual void *getMainBrowserDisp() { return (void *) mainBrowser; }
  virtual int getFrameDispIdx(void * disp);
  virtual void *getFrameBrowserDisp(int idx) { return (void *)frames.enumItem(idx); }
  virtual void *getDefaultCommandTarget() { return m_defaultcommandtarget; }
  virtual void *getDefaultClientSite() { return m_defaultclientsite; }
  void setDecorator(IDocHostUIHandlerDispatch * dec) { m_decorator = dec; }
  void setSite(IOleClientSite * site) { m_clientsite = site; }
  void *getContainer() { return contain; }
  void removeSite();
  void connect();
  void disconnect();
protected:
  virtual void event_onBeforeNavigate(const char *url, int flags, const char *targetframename, int *cancelnavigation);
  virtual void event_onDocumentComplete(const char *url);
  virtual void event_onNavigateComplete(const char *url){};
  virtual void event_onFrameNavigateComplete(const char *url, int idx){};
  virtual void event_onStatusTextChange(const char *val) { }
  virtual void event_onTitleChange(const char *val) { }
  virtual void event_onFrameBeforeNavigate(const char *url, int flags, const char *targetframename, int *cancelnavigation, int idx);
  virtual void event_onFrameDocumentComplete(const char *url, int idx);
  virtual void event_onProgressChange(long nProgress,long nProgressMax) {}
  virtual void event_onNewWindow(void **disp, int * cancel) { if (cancel) *cancel = 1; }
  virtual void event_onNavigateError(const char *url, int status, const char *targetframename, int *cancel) { }
  virtual void event_onFrameNavigateError(const char *url, int status, const char *targetframename, int *cancel, int idx);
  virtual void event_OnSetSecureLockIcon(long type) { }

  virtual int initBrowserStuff();
  virtual void freeBrowserStuff();
  virtual void onScrollbarsFlagTimer();
  virtual void resetFrames() {  frames.removeAll(); }
  virtual void notNavigated() { noinitnav = 1; }
  virtual void notConnected() { noinitconnect = 1; }
public:
#ifdef MULTIPLE_BROWSERWND
  static int Initialize();
  static void Shutdown();
#endif
  static void * GetZoneHelper();
private:
  virtual int doSetTargetName(const char *name);
  virtual int doSetScrollbars();

  virtual void updateTargetName();
  virtual void updateScrollbars();

  int browser_type;

//  IHTMLDocument2 *id;
  IWebBrowser2 *iwebbrowser;
  IDispatch * mainBrowser;
  IAxWinHostWindow *ihostwnd;
  IUnknown *iunk,*contain;
#ifndef MULTIPLE_BROWSERWND
  BOOL oleOk,atlInitialized;
#else
  static BOOL oleOk;
  static BOOL atlInitialized;
#endif
  static IInternetZoneManager *pZoneHelper;
  int noinitnav;
  int noinitconnect;
  String homepage;
  String deferednavigate;
  String targetname;
  String curpage;
  int timerid;
  int timerset1;
  int timerset2;
  int silent, accept_dnd;
  PtrList<MiniBrowserCallback> callbacks;
  PtrList<IDispatch> frames;
  int scrollbarsflag;
  IDocHostUIHandlerDispatch * m_decorator;
  IOleClientSite * m_clientsite;
  IOleClientSite * m_defaultclientsite;
  IOleCommandTarget * m_defaultcommandtarget;
  int m_events_connected;
};

#endif
