#ifndef _OPENFILE_H
#define _OPENFILE_H

#include <bfc/ptrlist.h>
#include <bfc/memblock.h>
#include <bfc/string/string.h>

class RootWnd;

class OpenFileWnd /*CUT : public Modal*/ {
public:
  OpenFileWnd(const char *ident=NULL, const char *default_dir=NULL);
  ~OpenFileWnd();

  int getOpenFile(RootWnd *parWnd, const char *ext, int chosenfilter=0, int multisel=0);
  int getNumFilenames() const { return filenames.getNumItems(); }
  const char *enumFilename(int i);
  const char *getDirectory() { return directory; }

private:
  // for internationalization
  int getOpenFileW(RootWnd *parWnd, const char *ext, int chosenfilter=0, int multisel=0);

  String identifier;
  String defaultdir;
  PtrListQuickSorted<String, StringComparator> filenames;
  String directory;
};

#if 0
class DefOpen : public svc_fileSelectorI {
public:
  static const char *getServiceName() { return "Default File Selector"; }

  DefOpen();
  virtual ~DefOpen();

  virtual int testPrefix(const char *prefix);
  virtual const char *getPrefix();
  virtual int setExtList(const char *ext);
  virtual int runSelector(RootWnd *parWnd, int type, int allow_multiple, const char *ident=NULL, const char *default_dir=NULL);
  virtual int getNumFilesSelected();
  virtual const char *enumFilename(int n);
  virtual const char *getDirectory() { return directory; }

private:
  int _type;
  OpenFileWnd *op;
  MemBlock<char> extlist;
  String directory;
};
#endif

#endif
