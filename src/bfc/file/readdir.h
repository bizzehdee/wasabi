#ifndef _READDIR_H
#define _READDIR_H

#if !defined(WIN32) && !defined(LINUX)
#error port me
#endif

#include <bfc/common.h>
#include <bfc/string/string.h>

/* intended use:
  ReadDir dir(path);
  while (dir.next()) {
    const char *fn = dir.getFilename();
  }
*/

class ReadDir {
public:
  ReadDir(const char *path, const char *match=NULL, BOOL skipdots=TRUE);
  ~ReadDir();

  int next();	// done when returns 0
  const char *getFilename();
  int isDir();	// if current file is a dir

  int isReadonly();	// if current file is readonly
  int isSystemFile();
  int isEncrypted();
  int isCompressed();
  int isTempFile();
  int isNormalFile();

  __int64 getFileSize();

  int isDotDir();	// if given dir iteself is being enumerated (usually ".")
  int isDotDotDir();	// if parent dir of cur dir is showing (usually "..")

  const char *getPath() { return path; }

private:
  String path, match;
  int skipdots, first;
//PORT
#ifdef WIN32
  HANDLE files;
  WIN32_FIND_DATA data;
  WIN32_FIND_DATAW dataW; // (shrug) so we have two?  so what?
#if UTF8
  int utf16;
#endif
  String filename8;
#endif
#ifdef LINUX
  DIR *d;
  struct dirent *de;
  struct stat st;
#endif
};

#endif
