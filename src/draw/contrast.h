#ifndef WASABI_CONTRAST_H
#define WASABI_CONTRAST_H
/*
  A simple method to apply a linear contrast effect on a range of pixels,
  making light things lighter and dark things darker.
*/

// 10-may-2004 BU created

#include <bfc/std.h>

void applyContrast(ARGB32 *bits, int w, int h, int pct);

#endif
