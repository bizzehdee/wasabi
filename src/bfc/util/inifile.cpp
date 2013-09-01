#include <precomp.h>

#include "inifile.h"
#include <bfc/guid.h>

#ifndef WASABI_PLATFORM_WIN32
#include "profile.h"
#endif 

IniFile::IniFile(const char *_filename) : filename(_filename) { }

void IniFile::deleteKey(const char *section, const char *tagname) {
  setString(section, tagname, NULL);
}

void IniFile::setString(const char *section, const char *tagname, const char *val) {
  WritePrivateProfileString(section, tagname, val, filename);
}

char *IniFile::getString(const char *section, const char *tagname, char *buf, int buflen, const char *default_val) {
  GetPrivateProfileString(section, tagname, default_val, buf, buflen, filename);
  return buf;
}

String IniFile::getString(const char *section, const char *tagname, const char *default_val) {
  char buf[WA_MAX_PATH]="";
  getString(section, tagname, buf, WA_MAX_PATH-1, default_val);
  return String(buf);
}

void IniFile::setInt(const char *section, const char *tagname, int val) {
  setString(section, tagname, StringPrintf(val));
}

int IniFile::getInt(const char *section, const char *tagname, int default_val) {
  char buf[MAX_PATH];
  getString(section, tagname, buf, sizeof(buf), StringPrintf(default_val));
  return ATOI(buf);
}

void IniFile::setIntHex(const char *section, const char *tagname, int val) {
  setString(section, tagname, StringPrintf("%x", val));
}

int IniFile::getIntHex(const char *section, const char *tagname, int default_val) {
  char buf[MAX_PATH];
  getString(section, tagname, buf, sizeof(buf), StringPrintf("%x", default_val));
  int ret = 0;
  SSCANF(buf, "%x", &ret);
  return ret;
}

bool IniFile::getBool(const char *section, const char *tagname, int default_val) {
  char buf[MAX_PATH];
  getString(section, tagname, buf, sizeof(buf), default_val ? "true" : "false");
  if (STRCASEEQL(buf, "true")) return 1;
  return 0;
}
  
void IniFile::setBool(const char *section, const char *tagname, int val) {
  setString(section, tagname, val ? "true" : "false");
}

GUID IniFile::getGuid(const char *section, const char *tagname, GUID default_val) {
  char buf[MAX_PATH];
  getString(section, tagname, buf, sizeof(buf), StringPrintf(default_val));
  return (GUID)Guid(buf);
}

void IniFile::setGuid(const char *section, const char *tagname, const GUID &val) {
  setString(section, tagname, StringPrintf(val));
}

RECT IniFile::getRect(const char *section, const char *tagname, const RECT default_val) {
  char buf[MAX_PATH]="";
  getString(section, tagname, buf, sizeof(buf), StringPrintf("%d %d %d %d", default_val.left, default_val.top, default_val.right, default_val.bottom));
  RECT ret = default_val;
  SSCANF(buf, "%d %d %d %d", &ret.left, &ret.top, &ret.right, &ret.bottom);
  return ret;
}

void IniFile::setRect(const char *section, const char *tagname, const RECT &rect) {
  setString(section, tagname, StringPrintf("%d %d %d %d", rect.left, rect.top, rect.right, rect.bottom));
}
