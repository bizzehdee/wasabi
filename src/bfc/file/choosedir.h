#ifndef _CHOOSEDIR_H
#define _CHOOSEDIR_H

class ChooseDir {
public:
  ChooseDir(OSWINDOWHANDLE parent, int allow_recursive=0);

  int choose(const char *title);

  const char *getResult();

  int getRecursiveSelected();

private:
  OSWINDOWHANDLE parWnd;
  int allowRecursive;
  String retval;
};

#endif
