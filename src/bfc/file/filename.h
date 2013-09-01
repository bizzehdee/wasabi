#ifndef _FILENAME_H
#define _FILENAME_H

#include <bfc/string/string.h>
//CUT #include "playstring.h"
#include <bfc/dispatch.h>

// a simple class to drag-and-drop filenames around

#define DD_FILENAME "DD_Filename v1"

class Filename : public Dispatchable {
public:
  const char *getFilename();
  operator const char *() { return getFilename(); }

  static const char *dragitem_getDatatype() { return DD_FILENAME; }

protected:
  enum {
    GETFILENAME=100,
  };
};

inline const char *Filename::getFilename() {
  return _call(GETFILENAME, (const char *)NULL);
}

// a basic implementation to use
class FilenameI : public Filename, public String {
public:
  FilenameI(const char *nf) : String(nf) { }
  const char *getFilename() { return getValue(); }

protected:
  FilenameI(const FilenameI &fn) {}
  FilenameI& operator =(const FilenameI &ps) { return *this; }
  RECVS_DISPATCH;
};

// another implementation that doesn't provide its own storage
class FilenameNC : public Filename {
public:
  FilenameNC(const char *str) { fn = str; }
  const char *getFilename() { return fn; }

protected:
  RECVS_DISPATCH;

private:
  const char *fn;
};

#if 0//CUT
// another implementation that uses the central playstring table
class FilenamePS : public Filename, private Playstring {
public:
  FilenamePS(const char *str) : Playstring(str) {}
  const char *getFilename() { return getValue(); }

protected:
  FilenamePS(const FilenamePS &fn) {}
  FilenamePS& operator =(const FilenamePS &ps) { return *this; }

  RECVS_DISPATCH;
};
#endif

#endif
