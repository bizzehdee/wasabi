#ifndef __FILTEREDCOLOR_H
#define __FILTEREDCOLOR_H

#include "string.h"

class COMEXP FilteredColor {
public:
  FilteredColor(COLORREF _color=0, const char *colorgroupname="");
  virtual ~FilteredColor();

  virtual void setColor(COLORREF _color);
  virtual void setColorGroup(const char *group);
  COLORREF getColor();
  COLORREF *getColorRef();
  virtual const char *getColorName() { return NULL; }

private:
  void ensureFiltered();

  COLORREF color;
  COLORREF filteredcolor;
  String group;
  int need_filter;
  static int *skin_iterator;
  int latest_iteration;
};


#endif
