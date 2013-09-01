#ifndef _SAVEFILE_H
#define _SAVEFILE_H

//CUT #include "common.h"

//CUT #include "modal.h"
#include <bfc/string/string.h>

class RootWnd;

class SaveFileWnd /*CUT : public Modal*/ {
public:
  SaveFileWnd(const char *ident=NULL);
/**
  Sets the initial directory for the picker. If force is FALSE, this directory
  will only be used if no previous directory has been saved for the indent.
  If TRUE, it will always be used.
*/
  void setInitialDir(const char *dir, int force=FALSE);
  int getSaveFile(OSWINDOWHANDLE parent, const char *ext, const char *suggext);
//  int getSaveFile(RootWnd *parent, const char *ext, const char *suggext);
//  int getSaveFileW(RootWnd *parent, const char *ext, const char *suggext);
  const char *getFilename() { return filename; }

private:
  String identifier;
  String filename;
  String initial_dir;
  int force_initial_dir;
};

#endif
