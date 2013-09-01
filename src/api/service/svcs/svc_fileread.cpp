#include <precomp.h>

#include "svc_fileread.h"

#define CBCLASS svc_fileReaderI
START_DISPATCH;
  CB(ISMINE, isMine);
  CB(OPEN, open);
  CB(READ, read);
  CB(WRITE, write);
  VCB(CLOSE, close);
  VCB(ABORT, abort);
  CB(GETLENGTH, getLength);
  CB(GETPOS, getPos);
  CB(CANSEEK, canSeek);
  CB(SEEK, seek);
  CB(HASHEADERS,hasHeaders);
  CB(GETHEADER,getHeader);
  CB(EXISTS,exists);
  CB(REMOVE,remove);
  CB(REMOVEUNDOABLE,removeUndoable);
  CB(MOVE,move);
  CB(COPY,copy);
  CB(BYTESAVAILABLE,bytesAvailable);
  VCB(SETMETADATACALLBACK,setMetaDataCallback);
  CB(CANPREFETCH,canPrefetch);
  CB(CANSETEOF, canSetEOF);
  CB(SETEOF, setEOF);
END_DISPATCH;
#undef CBCLASS

