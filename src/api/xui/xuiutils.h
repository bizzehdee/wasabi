#ifndef XUIUTILS_H
#define XUIUTILS_H

class DefinitionsFramework;

int parseBoolAttribute(const char *value, int allowinherit=0);
int parseColorAttribute(const char *color, COLORREF *colorValue);
void parseColorAndShadowAttribute(const char *colorattr, COLORREF *color, COLORREF *shadow, int *gotcolor, int *gotshadow);
int parseFontStyleAttribute(const char *style);
void parseCSVIntAttribute(const char *values, int *ret, int nvals, int defaultvalue);
int parseBGStyleAttribute(const char *value);
const char *unrelativizePath(const char *file);
const char *getImageFile(DefinitionsFramework *manager, const char *image);

#endif //XUIUTILS_H