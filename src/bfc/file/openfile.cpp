#include "precomp.h"

#include <windows.h>
#include <commdlg.h>
#include <crtdbg.h>

#include "openfile.h"

#include <bfc/assert.h>
#include <wnd/rootwnd.h>

#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in openfile.cpp -- Things might be screwy till it's all debugged?")
#endif
# include <bfc/string/encodedstr.h>
# include <api/service/svcs/svc_stringconverter.h>
#else
#error need fix for non UTF8
#endif

OpenFileWnd::OpenFileWnd(const char *ident, const char *default_dir)
  : identifier(ident) , defaultdir(default_dir) { }

OpenFileWnd::~OpenFileWnd() {
  filenames.deleteAll();
}

const int openfile_buflen = 256 * 1024 - 128;

int OpenFileWnd::getOpenFile(RootWnd *parWnd, const char *ext, int chosenfilter, int multisel) {
#if UTF8
  if (Std::encodingSupportedByOS(SvcStrCnv::UTF16)) {
    int ret = getOpenFileW(parWnd, ext, chosenfilter, multisel);
    // If ret returns -1, the service is not available, we will call our own
    // OSNative translation failure routines below:
    if (ret != -1) {
      return ret;
    }
  }
#endif
  char savedir[WA_MAX_PATH];
  Std::getCurDir(savedir, WA_MAX_PATH);

  filenames.deleteAll();
  char *buf = (char *)MALLOC(openfile_buflen);
  buf[0] = 0;
  ASSERT(ext != NULL);
  ASSERT(chosenfilter >= 0);
  OPENFILENAME ofn;
  ofn.lStructSize = sizeof ofn;
  ofn.hwndOwner = parWnd->gethWnd();
  ofn.hInstance = NULL;
  ofn.lpstrFilter = ext;
  ofn.lpstrCustomFilter = NULL;
  ofn.nMaxCustFilter = 0;
  ofn.nFilterIndex = chosenfilter;
  ofn.lpstrFile = buf;
  ofn.nMaxFile = openfile_buflen;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;

  String initDir;

  String tname;
  if (identifier != NULL) {
    tname.printf("Recent directories/OpenFile/%s", identifier.getValue());
    char dir[WA_MAX_PATH]="";
    WASABI_API_CONFIG->getStringPublic(tname, dir, WA_MAX_PATH, "");
    if (*dir) 
      initDir = dir;
    else 
      initDir = defaultdir;
  } else {
    if (!defaultdir.isempty())
      initDir = defaultdir;
  }

  int failed = 0;
  const char *theInitialDir = NULL;
  EncodedStr dirOSenc;
  int retcode = dirOSenc.convertFromUTF8(SvcStrCnv::OSNATIVE, initDir);
  if (retcode == SvcStrCnv::ERROR_UNAVAILABLE) {
    failed = 1;
    theInitialDir = initDir;
  } else { 
    theInitialDir = reinterpret_cast<char *>(dirOSenc.getEncodedBuffer());
  }

  ofn.lpstrInitialDir = theInitialDir;

  ofn.lpstrTitle = NULL;
  ofn.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
  if (multisel) ofn.Flags |= OFN_ALLOWMULTISELECT|OFN_EXPLORER;
  ofn.nFileOffset = 0;
  ofn.nFileExtension = 0;
  ofn.lpstrDefExt = NULL;
  ofn.lCustData = 0;
  ofn.lpfnHook = NULL;
  ofn.lpTemplateName = NULL;
  int ret = GetOpenFileName(&ofn);

  if (ret) {
    // parse 'em all out
    if (!multisel) {
      // If we only have one null terminated string, just convert
      String buf8;
      if (failed) { 
        buf8 = buf;
      } else {
        EncodedStr bufOSstr(SvcStrCnv::OSNATIVE, buf, STRLEN(buf)+1, 0/*no delete*/);
        bufOSstr.convertToUTF8(buf8);
      }

//CUT was      filenames.addItem(new StringPrintf("file:%s", buf8));
      filenames.addItem(new String(buf8));
    } else {
      char *prefix = buf;
      char *pt = prefix+STRLEN(prefix)+1;

      String prefix8;
      if (failed) { 
        prefix8 = prefix;
      } else {
        EncodedStr prefixOSstr(SvcStrCnv::OSNATIVE, prefix, STRLEN(prefix)+1, 0/*no delete*/);
        prefixOSstr.convertToUTF8(prefix8);
      }
      
      // warning: inappropriate use of cleverness
      do {
        String pt8;
        if (failed) { 
          pt8 = pt;
        } else {
          EncodedStr ptOSstr(SvcStrCnv::OSNATIVE, pt, STRLEN(pt)+1, 0/*no delete*/);
          ptOSstr.convertToUTF8(pt8);
        }
//CUT        // danger: putting StringPrintf into String... safe in this case tho
//CUT was        filenames.addItem(new StringPrintf("file:%s%s%s", prefix8, *pt8 ? "\\" : "", pt8));
        filenames.addItem(new String(StringPrintf("%s%s%s", prefix8, *pt8 ? "\\" : "", pt8)));
        pt += STRLEN(pt)+1;
      } while (*pt);
    }
  }

  // get new cur dir & save it off
  if (identifier != NULL && ret) {
    char newdir[WA_MAX_PATH];
    Std::getCurDir(newdir, WA_MAX_PATH);
    directory = newdir;

    WASABI_API_CONFIG->setStringPublic(tname, newdir);
  }

  Std::setCurDir(savedir);

  FREE(buf);
  return ret;
}

int OpenFileWnd::getOpenFileW(RootWnd *parWnd, const char *ext, int chosenfilter, int multisel) {
  int ret = 0;
#if UTF8

  // The ultimate top level retrohack. (sigh).
  // Test to see if the service is available.  If not, return -1.
  // The OSNATIVE codepath will include the proper failure handling.
  StringConverterEnum myServiceEnum(SvcStrCnv::UTF16);  // _ASSUME_ that there is a converter for U16 (we should, but still...)
  svc_stringConverter *myConv = myServiceEnum.getFirst();
  if (myConv != NULL) {
    myServiceEnum.release(myConv);
  } else {
    return -1;
  }


  char savedir[WA_MAX_PATH];
  Std::getCurDir(savedir, WA_MAX_PATH);

  filenames.deleteAll();
  WCHAR *buf16 = (WCHAR *)MALLOC(openfile_buflen * 2);
  buf16[0] = 0;
  ASSERT(ext != NULL);
  ASSERT(chosenfilter >= 0);

  // convert multi const char* ext to U16 (Ascii7 assumptive)
  int thisz = 0, lastz = 0;
  const char *px;
  WCHAR *pr, *ext16 = static_cast<WCHAR *>(MALLOC(WA_MAX_PATH));
  for (px = ext, pr = ext16; *px | (lastz); px++, pr++) {
    lastz = (thisz)?1:0;
    *pr = (*px) & 0xFF;
    thisz = (*pr)?1:0;
  }
  *pr = *px; // the last 0

  OPENFILENAMEW ofnW;
  ofnW.lStructSize = sizeof ofnW;
  ofnW.hwndOwner = parWnd->gethWnd();
  ofnW.hInstance = NULL;
  ofnW.lpstrFilter = ext16;
  ofnW.lpstrCustomFilter = NULL;
  ofnW.nMaxCustFilter = 0;
  ofnW.nFilterIndex = chosenfilter;
  ofnW.lpstrFile = buf16;
  ofnW.nMaxFile = openfile_buflen; // in characters!
  ofnW.lpstrFileTitle = NULL;
  ofnW.nMaxFileTitle = 0;

  // Figure out the initial directory in UTF8
  const char *initDir8 = NULL;
  String tname;
  if (identifier != NULL) {
    tname.printf("Recent directories/OpenFile/%s", identifier.getValue());
    char dir[WA_MAX_PATH]="";
    WASABI_API_CONFIG->getStringPublic(tname, dir, WA_MAX_PATH, "");
    if (*dir) 
      initDir8 = dir;
    else 
      initDir8 = defaultdir;
  } else {
    if (!defaultdir.isempty())
      initDir8 = defaultdir;
  }

  // And then convert it when you're done to UTF16.
  WCHAR *initDir16 = NULL;
  EncodedStr initDir16enc;
  if (initDir8 != NULL) {
    int written = initDir16enc.convertFromUTF8(SvcStrCnv::UTF16, String(initDir8));
    if (written > 0) {
       initDir16 = reinterpret_cast<WCHAR *>(initDir16enc.getEncodedBuffer());
    } else {
      return -1;
    }
  }

  // And then stuff it here.  Phew!
  ofnW.lpstrInitialDir = initDir16;

  ofnW.lpstrTitle = NULL;
  ofnW.Flags = OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;
  if (multisel) ofnW.Flags |= OFN_ALLOWMULTISELECT|OFN_EXPLORER;
  ofnW.nFileOffset = 0;
  ofnW.nFileExtension = 0;
  ofnW.lpstrDefExt = NULL;
  ofnW.lCustData = 0;
  ofnW.lpfnHook = NULL;
  ofnW.lpTemplateName = NULL;

  ret = GetOpenFileNameW(&ofnW);

  // Okay, at this point we have the string in UTF16 widechar format.
  // Now we downconvert everything to UTF8 and pass our information to the engine API's

  if (ret) {
    // parse 'em all out
    if (!multisel) {
      // If we only have one null terminated string, just convert
      EncodedStr buf16str(SvcStrCnv::UTF16, buf16, openfile_buflen * 2, 0/*no delete*/);
      String buf8;
      buf16str.convertToUTF8(buf8);
//CUT was      filenames.addItem(new StringPrintf("file:%s", buf8));
      filenames.addItem(new String(buf8));
    } else {
      // Otherwise we suck and must parse multiterminated strings for conversion, by hand.
      WCHAR *prefix16 = buf16;
      WCHAR *pt = prefix16+WSTRLEN(prefix16)+1;

      EncodedStr prefix16str(SvcStrCnv::UTF16, prefix16, (WSTRLEN(prefix16)+1) * 2, 0/*no delete*/);
      String prefix8;
      prefix16str.convertToUTF8(prefix8);      
      do {
        if (*pt) {
          EncodedStr buf16str(SvcStrCnv::UTF16, pt, (WSTRLEN(pt)+1) * 2, 0/*no delete*/);
          String buf8;
          buf16str.convertToUTF8(buf8);
//CUT was          filenames.addItem(new StringPrintf("file:%s%s%s", prefix8, "\\", buf8));
          filenames.addItem(new String(StringPrintf("%s%s%s", prefix8, "\\", buf8)));
        } else {
//CUT was          filenames.addItem(new StringPrintf("file:%s", prefix8));
          filenames.addItem(new String(prefix8));
        }
        pt += WSTRLEN(pt)+1;
      } while (*pt);
    }
  }

  // get new cur dir & save it off
  if (identifier != NULL && ret) {
    char newdir[WA_MAX_PATH];
    Std::getCurDir(newdir, WA_MAX_PATH);

    WASABI_API_CONFIG->setStringPublic(tname, newdir);
  }

  Std::setCurDir(savedir);

  FREE(buf16);
  FREE(ext16);
#endif
  return ret;
}

const char *OpenFileWnd::enumFilename(int i) {
  String *str = filenames.enumItem(i);
  return (str != NULL) ? str->getValue() : NULL;
}

#if 0
DefOpen::DefOpen() {
  op = NULL;
  _type = 0;
}

DefOpen::~DefOpen() {
  delete op;
}

int DefOpen::testPrefix(const char *prefix) {
  if (STRCASEEQL(prefix, "files")) return 1;
  return 0;
}

const char *DefOpen::getPrefix() {
  return "files";
}

int DefOpen::setExtList(const char *ext) {
  extlist.setSize(0);
  if (ext == NULL || *ext == '\0') return 0;
  for (int i = 1; ; i++) {
    if (ext[i-1] == 0 && ext[i] == 0) break;
  }
  int len = i+1;
  extlist.setSize(len);
  MEMCPY(extlist.getMemory(), ext, len);

  return 1;
}

int DefOpen::runSelector(RootWnd *parWnd, int type, int allow_multiple, const char *ident, const char *default_dir) {
  directory = default_dir;
  delete op; op = NULL;
  _type = type;
  if (_type == FileSel::OPEN) {
    op = new OpenFileWnd(ident, default_dir);
    const char *ext = extlist.getMemory();
//    if (ext == NULL) ext = "*.*";
    int r = op->getOpenFile(parWnd, ext, 0, allow_multiple);
    directory = op->getDirectory();
    return r;
  } else {
    return 0;
  }
}

int DefOpen::getNumFilesSelected() {
  if (_type == FileSel::OPEN) {
    return op ? op->getNumFilenames() : 0;
  } else {
    return 0;
  }
}

const char *DefOpen::enumFilename(int n) {
  if (_type == FileSel::OPEN) {
    return op ? op->enumFilename(n) : NULL;
  } else {
    return 0;
  }
}
#endif
