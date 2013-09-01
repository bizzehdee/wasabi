#ifndef _BLENDRECT_H
#define _BLENDRECT_H

#include "argb32.h"

// does not clip
void blendRect(ARGB32 *src, int srcw, int srch, ARGB32 *dest, int destx, int desty, int destw, int desth, int alpha);

#endif
