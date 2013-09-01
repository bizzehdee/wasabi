#ifndef _INIFILE_H
#define _INIFILE_H

#include <bfc/string/string.h>

class IniFile {
public:
  IniFile(const char *_filename);

  void deleteKey(const char *section, const char *tagname);

  void setString(const char *section, const char *tagname, const char *val);
  char *getString(const char *section, const char *tagname, char *buf, int buflen, const char *default_val = ""); // returns buf
  String getString(const char *section, const char *tagname, const char *default_val="");

  void setInt(const char *section, const char *tagname, int val);
  int getInt(const char *section, const char *tagname, int default_val = 0);

  void setIntHex(const char *section, const char *tagname, int val);
  int getIntHex(const char *section, const char *tagname, int default_val = 0);

  bool getBool(const char *section, const char *tagname, int default_val = 0);
  void setBool(const char *section, const char *tagname, int val);

  GUID getGuid(const char *section, const char *tagname, GUID default_val = INVALID_GUID);
  void setGuid(const char *section, const char *tagname, const GUID &val);

  RECT getRect(const char *section, const char *tagname, const RECT default_val=Std::makeRect());
  void setRect(const char *section, const char *tagname, const RECT &rect);

private:
  String filename;
};

#endif
