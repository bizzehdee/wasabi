#include "precomp.h"
#include "filename.h"

#define CBCLASS FilenameI
START_DISPATCH;
  CB(GETFILENAME, getFilename);
END_DISPATCH;
#undef CBCLASS

#define CBCLASS FilenameNC
START_DISPATCH;
  CB(GETFILENAME, getFilename);
END_DISPATCH;
#undef CBCLASS

#if 0
#define CBCLASS FilenamePS
START_DISPATCH;
  CB(GETFILENAME, getFilename);
END_DISPATCH;
#undef CBCLASS
#endif
