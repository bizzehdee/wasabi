#ifndef _SKINCLR_H
#define _SKINCLR_H

#include <bfc/common.h>
#include <bfc/std.h>
#include "filteredcolor.h"

// note: only pass in a const char *
class SkinColor : public FilteredColor {
public:
  explicit SkinColor(const char *name=NULL, const char *colorgroup=NULL);

  DWORD v();
  operator int() { return v(); }

  void setElementName(const char *name);
  const char *operator =(const char *name) { setElementName(name); return name;}

private:
  const char *name;
  DWORD *val;
  static int *skin_iterator;
  int latest_iteration;
  COLORREF filteredcolor;
};

#endif
