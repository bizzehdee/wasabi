#ifndef __SKINPARSE_H
#define __SKINPARSE_H

class Skin {
public:
  static const char *getDefaultSkinPath();
  static void unloadResources();
  static void newSkinPart();
  static void reloadResources();
};


#endif


