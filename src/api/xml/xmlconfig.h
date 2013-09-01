/*

  Nullsoft WASABI Source File License

  Copyright 1999-2003 Nullsoft, Inc.

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.

*/
#ifndef _XMLCONFIG_H
#define _XMLCONFIG_H

#include <bfc/string/string.h>
#include <bfc/pair.h>
#include <bfc/critsec.h>

class StringPair;

class XmlConfigFile {
public:
  XmlConfigFile(const char *section, const char *name);
  ~XmlConfigFile();

  static void initialize();

  void setInt(const char *name, int val);
  int getInt(const char *name, int default_val);

  void setString(const char *name, const char *str);
  int getString(const char *name, char *buf, int buf_len, const char *default_str);

  int getStringLen(const char *name);

private:
  String sectionname, prettyname;
  StringPair *getPair(const char *name);
  StringPair *makePair(const char *name, const char *value);
  CriticalSection cs;
};

#endif
