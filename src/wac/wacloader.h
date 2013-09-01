#ifndef WAC_LOADER_H
#define WAC_LOADER_H

#include "waContainer.h"

class WacLoader {
public:
  WacLoader(const char *subdir="wacs", const char *ext="*.wac");
  virtual ~WacLoader();

  int getNumWacs() { return m_wacs.getNumItems(); }
  waContainer *enumWac(int i) { return m_wacs.enumItem(i); }

private:
  PtrList<waContainer> m_wacs;
  String m_wacdir;
};

#endif
