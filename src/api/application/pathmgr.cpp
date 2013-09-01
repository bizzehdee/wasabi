#include <precomp.h>
#include "pathmgr.h"
#include <api/api.h>
#include <bfc/util/inifile.h>

#if !defined(WIN32) && !defined(LINUX)
#error port me
#endif

#ifdef WIN32
#include <shlobj.h>
#endif

#ifdef WASABI_PLATFORM_DARWIN
#include <CoreServices/CoreServices.h>
#endif

#define INIFILE "wasabi.ini"

// by default store shit in user's home dir
#define DEFAULT_MULTIUSER TRUE

class OrigPath {
public:
  OrigPath() {
    MEMZERO(buf, WA_MAX_PATH);
    Std::getCurDir(buf, WA_MAX_PATH);
    buf[WA_MAX_PATH-1] = '\0';
  }
  char buf[WA_MAX_PATH];
};
static OrigPath original_path;

static int checkMU() {
  String appp = WASABI_API_APP->path_getAppPath();
  Std::fileInfoStruct fis;
  Std::getFileInfos(appp, &fis);
  if (fis.readonly) return TRUE;
#if 0//eh, let's just trust that the dir would be readonly
#ifdef WIN32
  String apdir = appp;
  apdir.trunc(3);	// "c:\"
  if (GetDriveType(apdir) == DRIVE_CDROM) return TRUE;
#endif
#endif
  return IniFile(StringPrintf("%s%s%s", WASABI_API_APP->path_getAppPath(), DIRCHARSTR, INIFILE)).getBool("Wasabi", "MultiUser", DEFAULT_MULTIUSER);
}

static void makeUserDir(String &dir) {
#ifdef WIN32
  ITEMIDLIST *lpiil;
  int r = SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA,  &lpiil);
  if (r != S_OK) {
    ASSERTALWAYS("no home dir found");
  }
  char buf[MAX_PATH];
  r = SHGetPathFromIDList(lpiil, buf);
  if (r == FALSE) {
    ASSERTALWAYS("path fuct");
  }

  LPMALLOC ppm;
  if (CoGetMalloc(1, &ppm) == S_OK) {
    ppm->Free(lpiil);
    ppm->Release();
  }

  dir = buf;
  dir.cat(DIRCHARSTR);
  dir.cat(WASABI_API_APP->main_getAppName());
  Std::createDirectory(dir);
#endif
#ifdef LINUX
#ifdef WASABI_PLATFORM_DARWIN
  FSRef fsref;
  char buf[MAX_PATH];
  FSFindFolder(kUserDomain,kApplicationSupportFolderType,kCreateFolder,&fsref);
  FSRefMakePath(&fsref,buf,MAX_PATH);
  buf[MAX_PATH-1]=0;
  dir.cat(buf);  // ~/Library/Application Support"
  dir.cat(DIRCHARSTR);
  dir.cat(WASABI_API_APP->main_getAppName());
  Std::createDirectory(dir);  
#else
  dir = "~/";	// hee hee
#endif
#endif
//PORTME
  ASSERT(!dir.isempty());
}

const char *PathMgr::getUserSettingsPath() {
  static String ret;
  if (ret.isempty()) {
    if (!checkMU()) ret = WASABI_API_APP->path_getAppPath();
    else {
      // create per-user dir string
      makeUserDir(ret);
    }
  }
//ASSERT(!Std::isDirChar(ret.lastChar()));
  if (Std::isDirChar(ret.lastChar())) {
    ret.trunc(ret.len()-1);
  }
  return ret;
}

const char *PathMgr::getOriginalPath() {
  static String ret;	// don't let people overwrite our data
  if (ret.isempty()) {
    ret = original_path.buf;
  }
  if (Std::isDirChar(ret.lastChar())) {
    ret.trunc(ret.len()-1);
  }
  return ret;
}

#ifdef WASABI_COMPILE_COMPONENTS
const char *PathMgr::getComponentDataPath(GUID g) {
  // for now, we ignore GUID
  static String ret;
  if (ret.isempty()) {
    if (!checkMU()) {
      ret = WASABI_API_APP->path_getAppPath();
      ret.cat(DIRCHARSTR);
      ret.cat("Wacs");
      ret.cat(DIRCHARSTR);
      ret.cat("data");
    } else {
      // create per-user dir string
      makeUserDir(ret);
      ret.cat(DIRCHARSTR);
      ret.cat("data");
      Std::createDirectory(ret);
    }
  }
  return ret;
}
#endif

