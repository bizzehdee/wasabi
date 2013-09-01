#ifndef _SAVEPPM_H
#define _SAVEPPM_H

// this uses a nonstandard alpha saving format if wantalpha is TRUE. the
// ppmloader module can load it, but no other PPM programs will be able to.
int savePPM(const char *savefn, ARGB32 *bits, int w, int h, int wantalpha=FALSE);

#endif
