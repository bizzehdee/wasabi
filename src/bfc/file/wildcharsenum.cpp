// NONPORTABLE NONPORTABLE NONPORTABLE
#include <precomp.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "wildcharsenum.h"
#include <parse/pathparse.h>
#include <parse/paramparser.h>
#include <bfc/file/readdir.h>

#ifdef WIN32
#endif
#if UTF8
#ifdef WANT_UTF8_WARNINGS
#pragma CHAT("mig", "all", "UTF8 is enabled in wildcharsenum.cpp -- Things might be screwy till it's all debugged?")
#endif
# include <bfc/string/encodedstr.h>
#endif

WildcharsEnumerator::WildcharsEnumerator(const char *_selection) : selection(_selection) {
  // Test if the system supports U16 at initialization.
#if UTF8
  utf16 = Std::encodingSupportedByOS(SvcStrCnv::UTF16);
#endif
  // Then scan.
  rescan();
}

WildcharsEnumerator::~WildcharsEnumerator() {
  finddatalist.deleteAll();
}

int WildcharsEnumerator::getNumFiles() {
  return finddatalist.getNumItems();
}

const char *WildcharsEnumerator::enumFile(int n) {
  String path = finddatalist.enumItem(n)->path;
  if (!path.isempty()) {
    enumFileString.printf("%s%s", path.getValue(), finddatalist.enumItem(n)->filename.getValue());
    return enumFileString;
  }
  return finddatalist.enumItem(n)->filename;
}

void WildcharsEnumerator::rescan() {
  finddatalist.removeAll();
  ParamParser pp(selection, ";");
  for (int is = 0; is < pp.getNumItems(); is++) {
    String _selection = pp.enumItem(is);
    char p[WA_MAX_PATH]="";
    STRNCPY(p, _selection, WA_MAX_PATH-1);

    PathParser parse(_selection);
    String path = "";
    String mask = "";

    if (_selection.getValue() && Std::isDirChar(*_selection.getValue())) path += DIRCHARSTR;
    for (int i=0;i<parse.getNumStrings()-1;i++)
      path.cat(StringPrintf("%s%c", parse.enumString(i), Std::dirChar()));

    mask = parse.getLastString();

    // forcibly convert all slashes to proper directory separator char.
    char *q = p;
    while (*q) {
      if (*q == '/') *q = Std::dirChar();
      if (*q == '\\') *q = Std::dirChar();
      q++;
    }

    // enum files and store a list
    ReadDir rd(path, mask, TRUE);
    while (rd.next()) {
      finddatalist.addItem(new find_entry(rd.getPath(), rd.getFilename()));
    }
  }
}

int WildcharsEnumerator::isWildchars(const char *filename) {
  return (STRCHR(filename, '*') || STRCHR(filename, '?'));
}

