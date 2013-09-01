#ifndef _CONFIG_H
#define _CONFIG_H

#include <bfc/string/string.h>
#include <bfc/pair.h>
#include <bfc/critsec.h>

class StringPair : public Pair<String, String> {
public:
  StringPair(const char *a, const char *b) : Pair<String, String>(a, b) {}
};

class ConfigFile {
public:
  ConfigFile(const char *section, const char *name);
  ~ConfigFile();

  static void initialize();

  void setInt(const char *name, int val);
  int getInt(const char *name, int default_val);

  void setString(const char *name, const char *str);
  int getString(const char *name, char *buf, int buf_len, const char *default_str);

  int getStringLen(const char *name);

private:
  String sectionname, prettyname;
  String makeName(const char *name);	// prepends the section name
  StringPair *getPair(const char *name);
  StringPair *makePair(const char *name, const char *value);
  CriticalSection cs;
};

#endif
