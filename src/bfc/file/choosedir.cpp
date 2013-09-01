#include "precomp.h"
#ifdef WIN32
#include <windows.h>
#include <shlobj.h>
#endif

#include "choosedir.h"

//CUT #include "../bfc/std.h"
//CUT #include "../common/xlatstr.h"

#ifdef WIN32
#endif
#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in choosedir.cpp -- Things might be screwy till it's all debugged?")
#endif
# include <bfc/string/encodedstr.h>
#endif


static int recursiveSelected=1;
#ifdef WIN32
static HWND hwndButton;
static long (__stdcall *BrowseCheckBoxProc_old)(struct HWND__ *,unsigned int,unsigned int,long);
static RecycleString checkBoxTitle;

static LRESULT CALLBACK BrowseCheckBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg) {
    case WM_DESTROY:
      recursiveSelected=1;
      if (!SendMessage(hwndButton,BM_GETCHECK,0,0)) recursiveSelected=0;
    break;
  }
  return CallWindowProc(BrowseCheckBoxProc_old,hwnd,uMsg,wParam,lParam);
}

int CALLBACK WINAPI BrowseCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) {
  switch (uMsg) {
    case BFFM_INITIALIZED: {
      char buf[MAX_PATH];

#if UTF8
        if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
          // CODE FOR NT COMPATIBLE OS'S
          EncodedStr title16enc;
          int check = title16enc.convertFromUTF8(SvcStrCnv::UTF16, String(checkBoxTitle));
          if (check > 0) {
            wchar_t *title16 = static_cast<wchar_t *>(title16enc.getEncodedBuffer());
    				SetWindowTextW(hwnd,title16);
          } else if (check == SvcStrCnv::ERROR_UNAVAILABLE) {
    				SetWindowText(hwnd,checkBoxTitle);
          }
        } else {
          // CODE FOR 9x COMPATIBLE OS'S
          EncodedStr titleOSenc;
          int check = titleOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(checkBoxTitle));
          if (check > 0) {
            char *titleOS = static_cast<char *>(titleOSenc.getEncodedBuffer());
    				SetWindowText(hwnd,titleOS);
          } else if (check == SvcStrCnv::ERROR_UNAVAILABLE) {
    				SetWindowText(hwnd,checkBoxTitle);
          }
        }

#else // The code that was there before.
        // Set this to translate from UTF8
				SetWindowText(hwnd,checkBoxTitle);
#endif


      GetCurrentDirectory(MAX_PATH,buf);
      SendMessage(hwnd,BFFM_SETSELECTION,1,(long)buf);
      hwndButton=CreateWindowEx( 0,"WinampBrowseCheckBox",
                 /*_*/("Recurse subdirectories"),
                 BS_AUTOCHECKBOX|WS_TABSTOP|WS_CHILD|WS_VISIBLE,10,10,
                 200,20,hwnd,NULL,GetModuleHandle(NULL),0);
      if (recursiveSelected) SendMessage(hwndButton,BM_SETCHECK,1,0);
    }
  }
  return 0;
}
#endif//WIN32

ChooseDir::ChooseDir(OSWINDOWHANDLE wnd, int allow_recursive) {
  parWnd = wnd;
  allowRecursive = allow_recursive;
}

static void Shell_Free(void *thing) {
#ifdef WIN32
  IMalloc *m;
  SHGetMalloc(&m);
  m->Free(thing);
#endif
}

int ChooseDir::choose(const char *title) {
  char buf[WA_MAX_PATH];
#ifdef WIN32
  if(allowRecursive) {
    static int registered=0;
    if(!registered) {
      WNDCLASS wc={0,};
      GetClassInfo(GetModuleHandle(NULL),"Button",&wc);
    	wc.lpszClassName="WinampBrowseCheckBox";
	    BrowseCheckBoxProc_old=wc.lpfnWndProc;
	    wc.lpfnWndProc=BrowseCheckBoxProc;
	    RegisterClass(&wc);
      registered=1;
    }

    // This is a UTF8 value, translated above.
    checkBoxTitle=title;
  }

  // Moved outside the #if
  ITEMIDLIST *idlist;
#if UTF8
  int failed = 0;
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    // CODE FOR NT COMPATIBLE OS'S
    EncodedStr title16enc;
    int check = title16enc.convertFromUTF8(SvcStrCnv::UTF16, String(title));
    if (check == SvcStrCnv::ERROR_UNAVAILABLE) {
      failed = 1;
    } else { 
      BROWSEINFOW biW;
      WCHAR blah[MAX_PATH];
      biW.hwndOwner = parWnd;
      biW.pidlRoot = 0;
      biW.pszDisplayName = blah;
      biW.lpszTitle = static_cast<WCHAR *>(title16enc.getEncodedBuffer());
      biW.ulFlags = BIF_RETURNONLYFSDIRS;// | BIF_EDITBOX;
      biW.lpfn = allowRecursive?BrowseCallbackProc:NULL;
      biW.lParam = 0;
      biW.iImage = 0;

      idlist = SHBrowseForFolderW(&biW);
    }
  } else {
    // CODE FOR 9x COMPATIBLE OS'S
    EncodedStr titleOSenc;
    int check = titleOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, String(title));
    if (check == SvcStrCnv::ERROR_UNAVAILABLE) {
      failed = 1;
    } else { 
      BROWSEINFO bi;
      char blah[MAX_PATH];
      bi.hwndOwner = parWnd;
      bi.pidlRoot = 0;
      bi.pszDisplayName = blah;
      bi.lpszTitle = static_cast<char *>(titleOSenc.getEncodedBuffer());
      bi.ulFlags = BIF_RETURNONLYFSDIRS;// | BIF_EDITBOX;
      bi.lpfn = allowRecursive?BrowseCallbackProc:NULL;
      bi.lParam = 0;
      bi.iImage = 0;

      idlist = SHBrowseForFolder(&bi);
    }    
  }

  if (failed) {
    BROWSEINFO bi;
    char blah[MAX_PATH];
    bi.hwndOwner = parWnd;
    bi.pidlRoot = 0;
    bi.pszDisplayName = blah;
    bi.lpszTitle = title;
    bi.ulFlags = BIF_RETURNONLYFSDIRS;// | BIF_EDITBOX;
    bi.lpfn = allowRecursive?BrowseCallbackProc:NULL;
    bi.lParam = 0;
    bi.iImage = 0;

    idlist = SHBrowseForFolder(&bi);
  }
#else // The code that was there before.
  // And all the stuff below here needs alot of tuning for explicit W
  BROWSEINFO bi;
  char blah[MAX_PATH];
  bi.hwndOwner = parWnd;
  bi.pidlRoot = 0;
  bi.pszDisplayName = blah;
  bi.lpszTitle = title;
  bi.ulFlags = BIF_RETURNONLYFSDIRS;// | BIF_EDITBOX;
  bi.lpfn = allowRecursive?BrowseCallbackProc:NULL;
  bi.lParam = 0;
  bi.iImage = 0;

  idlist = SHBrowseForFolder(&bi);
#endif


  if (idlist == NULL) return 0;
  *buf = 0;


#if UTF8
  if (failed) {
    SHGetPathFromIDList(idlist, buf);
  } else {
    String buf8;
    if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
      // CODE FOR NT COMPATIBLE OS'S
      WCHAR buf16[WA_MAX_PATH];
      SHGetPathFromIDListW(idlist, buf16);
      EncodedStr buf16enc(SvcStrCnv::UTF16, buf16, WSTRLEN(buf16)+1, 0/*no delete*/);
      buf16enc.convertToUTF8(buf8);
    } else {
      // CODE FOR 9x COMPATIBLE OS'S
      char bufOS[WA_MAX_PATH];
      SHGetPathFromIDList(idlist, bufOS);
      EncodedStr bufOSenc(SvcStrCnv::OSNATIVE, bufOS, STRLEN(bufOS)+1, 0/*no delete*/);
      bufOSenc.convertToUTF8(buf8);
    }
    STRCPY(buf,buf8);
  }
#else
  SHGetPathFromIDList(idlist, buf);
#endif

  Shell_Free(idlist);

#else
  STRCPY(buf, "/");
  DebugString( "portme -- ChooseDir::choose\n" );
#endif

  retval = buf;

  return 1;
}

const char *ChooseDir::getResult() {
  return retval;
}

int ChooseDir::getRecursiveSelected() {
  return recursiveSelected;
}
