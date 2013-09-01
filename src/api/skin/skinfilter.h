#ifndef WASABI_SKINFILTER_H
#define WASABI_SKINFILTER_H

#include <bfc/std.h>

class ApplySkinFilters {
public:
  static void apply(const char *element_id, const char *forced_gammagroup, ARGB32 *bits, int w, int h, int bpp=32);
};

#endif
