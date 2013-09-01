#include <precomp.h>
#include "version.h"
#include <parse/pathparse.h>

static int buildno = -1;
static const char *WASABI_VERSION = "";

#define STRFILEVER     "1, 0, 0, 499\0"

static String appname, version_string;
#ifdef DARWINBUNDLE
static String bundleId;
#endif

void WasabiVersion::setAppName(const char *name) {
  appname = name;
}

const char *WasabiVersion::getAppName() {
  return appname;
}

const char *WasabiVersion::getVersionString() {
  if (appname.isempty()) appname="";
  if (version_string.isempty()) {
    if (!appname.isempty())
      version_string = appname.getValue();
    if (STRLEN(WASABI_VERSION)) {
      version_string.cat(" ");
      version_string.cat(WASABI_VERSION);
    }
  }
  return version_string;
}

void WasabiVersion::setBuildNumber(int buildno) {
  ::buildno = buildno;
}

unsigned int WasabiVersion::getBuildNumber() {
  if (buildno == -1) {
    PathParser pp(STRFILEVER, ",");
    buildno = ATOI(pp.enumString(3));
  }
  return buildno;
}

int WasabiVersion::versionStringToInt(const char *str) {
  if (str == NULL) return 0;
  PathParser pp(str, ",");
  return ATOI(pp.enumString(3));
}

#ifdef DARWINBUNDLE
void WasabiVersion::setBundleId(const char *bundleid) {
  bundleId = bundleid;
}

const char *WasabiVersion::getBundleId() {
  return bundleId;
}
#endif
