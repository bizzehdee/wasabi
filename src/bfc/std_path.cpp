#include "precomp.h"

#ifdef WIN32
#include <shlobj.h>
#endif

#include "std_path.h"

String StdPath::userHomeDir() {
  String ret;
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
  ret = buf;
#else
  ret = "~/";
#endif
  return ret;
}
