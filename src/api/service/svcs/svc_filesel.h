#ifndef _SVC_FILESEL_H
#define _SVC_FILESEL_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

class RootWnd;

class NOVTABLE svc_fileSelector : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::FILESELECTOR; }

  int testPrefix(const char *prefix) {
    return _call(TESTPREFIX, 0, prefix);
  }
  const char *getPrefix() {
    return _call(GETPREFIX, "");
  }
  int setTitle(const char *title) {
    return _call(SETTITLE, 0, title);
  }
  int setExtList(const char *ext) {
    return _call(SETEXTLIST, 0, ext);
  }
  int runSelector(RootWnd *parWnd, int type, int allow_multiple, const char *ident=NULL, const char *default_dir=NULL) {
    return _call(RUNSELECTOR, 0, parWnd, type, allow_multiple, ident, default_dir);
  }
  int getNumFilesSelected() {
    return _call(GETNUMFILESSELECTED, 0);
  }
  const char *enumFilename(int n) {
    return _call(ENUMFILENAME, "", n);
  }
  const char *getDirectory() {
    return _call(GETDIRECTORY, "");
  }

protected:
  enum {
    TESTPREFIX=0,
    GETPREFIX=10,
    SETTITLE=20,
    SETEXTLIST=30,
    RUNSELECTOR=40,
    GETNUMFILESSELECTED=50,
    ENUMFILENAME=60,
    GETDIRECTORY=70,
  };
};

namespace FileSel {
  enum {
    OPEN=1, SAVEAS=2,
  };
};


class NOVTABLE svc_fileSelectorI : public svc_fileSelector {
public:
  virtual int testPrefix(const char *prefix)=0;
  virtual const char *getPrefix()=0;
  virtual int setTitle(const char *title) { return 0; }
  virtual int setExtList(const char *ext) { return 0; }
  virtual int runSelector(RootWnd *parWnd, int type, int allow_multiple, const char *ident=NULL, const char *default_dir=NULL)=0;
  virtual int getNumFilesSelected()=0;
  virtual const char *enumFilename(int n)=0;
  virtual const char *getDirectory()=0;

protected:
  RECVS_DISPATCH;
};

#include <api/service/svc_enum.h>
#include <bfc/string/string.h>

class FileSelectorEnum : public SvcEnumT<svc_fileSelector> {
public:
  FileSelectorEnum(const char *_prefix="files") : prefix(_prefix) { }

protected:
  virtual int testService(svc_fileSelector *svc) {
    return prefix.isempty() || svc->testPrefix(prefix);
  }

private:
  String prefix;
};

#endif
