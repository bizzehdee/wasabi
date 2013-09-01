#ifndef _PATHMGR_H
#define _PATHMGR_H

#include <bfc/std.h>

// note that if someone changes the current directory in their constructor,
// we're going to get the wrong directory for original path. but only a
// bastard would do that. right?

namespace PathMgr {
  const char *getUserSettingsPath();
  const char *getOriginalPath();
#ifdef WASABI_COMPILE_COMPONENTS
  const char *getComponentDataPath(GUID g);
#endif
};

#endif
