#ifndef _VERSION_H
#define _VERSION_H

namespace WasabiVersion {
  void setAppName(const char *name);
  const char *getAppName();
  const char *getVersionString();
  void setBuildNumber(int buildno);
  unsigned int getBuildNumber();
  int versionStringToInt(const char *str);	// i.e. "1,0,0,34" from autobuild
#ifdef DARWINBUNDLE
  void setBundleId(const char *bundleid);
  const char *getBundleId();
#endif
};


#endif
