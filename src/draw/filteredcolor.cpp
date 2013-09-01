#include "precomp.h"

//CUT #include "../studio/api.h"
#include <wasabicfg.h>
#include "filteredcolor.h"

FilteredColor::FilteredColor(COLORREF _color, const char *colorgroupname) {
  color = _color;
  filteredcolor = _color;
  group = colorgroupname;
  skin_iterator = NULL;
  latest_iteration = -1;
  need_filter = 1;
}

FilteredColor::~FilteredColor() {
}

void FilteredColor::setColor(COLORREF _color) { 
  color = _color; 
  filteredcolor = color;
}

void FilteredColor::setColorGroup(const char *_group) {
  group = _group;
  need_filter = 1;
}

COLORREF FilteredColor::getColor() {
  ensureFiltered();
  return filteredcolor;
}

COLORREF *FilteredColor::getColorRef() {
  ensureFiltered();
  return &filteredcolor;
}

void FilteredColor::ensureFiltered() {
#ifndef WASABI_COMPILE_SKIN
  need_filter = 0;
  filteredcolor = color;
#else
  // fetch iterator pointer if necessary
  if (skin_iterator == NULL) {
    skin_iterator = WASABI_API_SKIN->skin_getIterator();
    ASSERT(skin_iterator != NULL);
  }

  // see if we're current
  if (*skin_iterator != latest_iteration) {
    need_filter = 1;	// pointer now invalid, must re-get
    latest_iteration = *skin_iterator;	// and then we'll be current
  }


  if (need_filter) {
    filteredcolor = WASABI_API_SKIN->filterSkinColor(color, getColorName(), group);
    need_filter = 0;
  }
#endif
}

int *FilteredColor::skin_iterator=NULL;
