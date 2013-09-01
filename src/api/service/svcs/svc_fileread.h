#ifndef _SVC_FILEREAD_H
#define _SVC_FILEREAD_H

#include <bfc/dispatch.h>
#include <api/service/services.h>

namespace SvcFileReader {
  enum {
    READ=1,
    WRITE=2,
    APPEND=4,
    PLUS=8,
    BINARY=16,
    TEXT=32,
  };
};

class MetaDataReaderCallback;

class NOVTABLE svc_fileReader : public Dispatchable {
public:
  static GUID getServiceType() { return WaSvc::FILEREADER; }

  int isMine(const char *filename, int mode=SvcFileReader::READ);//don't really open. returns -1 if "don't know until I open it"
  int open(const char *filename, int mode=SvcFileReader::READ);
  int read(char *buffer, int length);
  int write(const char *buffer, int length);
  void close();// safe to call even when not open

  int canSetEOF();
/**
  Asks the file reader to change the file length to newlen. Will fail if file
  was not opened for writing.
  @ret 1 on success, 0 on failure, -1 if operation is unsupported by this reader.
*/
  int setEOF(int newlen);

  void abort();
  
  int getLength();
  int getPos();
  
  int canSeek();
  int seek(int position);
  int bytesAvailable(int requested);

  int hasHeaders();
  const char *getHeader(const char *header);

  int exists(const char *filename);

  int remove(const char *filename);

  int removeUndoable(const char *filename);

  int move(const char *filename, const char *destfilename);

  int copy(const char *filename, const char *destfilename);

  int canPrefetch();

  void setMetaDataCallback(MetaDataReaderCallback *cb);

  enum {
    ISMINE=0,
    OPEN=10,
    READ=20,
    WRITE=30,
    CLOSE=40,
    ABORT=50,
    GETLENGTH=60,
    GETPOS=70,
    CANSEEK=80,
    SEEK=90,
    HASHEADERS=100,
    GETHEADER=110,
    EXISTS=120,
    REMOVE=130,
    REMOVEUNDOABLE=135,
    BYTESAVAILABLE=140,
  	SETMETADATACALLBACK=150,
    MOVE=160,
    COPY=165,
    CANPREFETCH=170,
    CANSETEOF=180,
    SETEOF=190,
  };
};

inline
int svc_fileReader::isMine(const char *filename, int mode) {
  return _call(ISMINE, -1, filename, mode);
}

inline int svc_fileReader::open(const char *filename, int mode) { 
  return _call(OPEN, 0, filename, mode); 
}

inline int svc_fileReader::read(char *buffer, int length) {
  return _call(READ, 0, buffer,length); 
}

inline int svc_fileReader::write(const char *buffer, int length) { 
  return _call(WRITE, 0, buffer,length); 
}

inline void svc_fileReader::close() { 
  _voidcall(CLOSE); 
}

inline int svc_fileReader::canSetEOF() {
  return _call(CANSETEOF, 0);
}

inline int svc_fileReader::setEOF(int newlen) {
  return _call(SETEOF, -1, newlen);
}

inline void svc_fileReader::abort() { 
  _voidcall(ABORT); 
}
  
inline int svc_fileReader::getLength() { 
  return _call(GETLENGTH, -1); 
}

inline int svc_fileReader::getPos() { 
  return _call(GETPOS, 0); 
}
  
inline int svc_fileReader::canSeek() { 
  return _call(CANSEEK, 0); 
}

inline int svc_fileReader::seek(int position) { 
  return _call(SEEK, 0, position); 
}

inline int svc_fileReader::bytesAvailable(int requested) { 
  return _call(BYTESAVAILABLE,requested,requested); 
}

inline int svc_fileReader::hasHeaders() { 
  return _call(HASHEADERS,0); 
}

inline const char *svc_fileReader::getHeader(const char *header) { 
  return _call(GETHEADER,(const char *)NULL,header); 
}

inline int svc_fileReader::exists(const char *filename) { 
  return _call(EXISTS,-1,filename); 
}

inline int svc_fileReader::remove(const char *filename) { 
  return _call(REMOVE,-1,filename);
}

inline
int svc_fileReader::removeUndoable(const char *filename) {
  return _call(REMOVEUNDOABLE, -1, filename);
}

inline int svc_fileReader::move(const char *filename, const char *destfilename) { 
  return _call(MOVE,-1,filename, destfilename);
}

inline int svc_fileReader::copy(const char *filename, const char *destfilename) {
  return _call(COPY,-1,filename, destfilename);
}

inline void svc_fileReader::setMetaDataCallback(MetaDataReaderCallback *cb) {
  _voidcall(SETMETADATACALLBACK,cb); 
}

inline int svc_fileReader::canPrefetch() { 
  return _call(CANPREFETCH,1); 
}

// derive from this one
class NOVTABLE svc_fileReaderI : public svc_fileReader {
public:
  virtual int isMine(const char *filename, int mode=SvcFileReader::READ) { return -1; }
  virtual int open(const char *filename, int mode=SvcFileReader::READ)=0; // return 1 on success
  virtual int read(char *buffer, int length)=0; // return number of bytes read (if < length then eof)
  virtual int write(const char *buffer, int length)=0; // return number of bytes written
  virtual void close()=0;//must be safe to call even when not open

  virtual int canSetEOF() { return 0; }
  virtual int setEOF(int newlen) { return -1; }

  virtual void abort() { } // tells the reader to abort its current prebuffering/reader

  virtual int getLength() { return -1; } // return -1 on unknown/infinite
  virtual int getPos()=0;

  virtual int canSeek() { return 0; }
  virtual int seek(int position) { return 0; }
  virtual int bytesAvailable(int requested) { return requested; }

  virtual int hasHeaders() { return 0; }
  virtual const char *getHeader(const char *header) { return (const char *)NULL; }

  virtual int exists(const char *filename) { return 0; } // return 1 if true, 0 if not, -1 if unknown

  virtual int remove(const char *filename) { return 0; } // return 1 on success, 0 on error, -1 if unsupported

  virtual int removeUndoable(const char *filename) { return -1; } // returns 1 on success, 0 on error, -1 if undoable deletes aren't supported

  virtual int move(const char *filename, const char *destfilename) { return 0; } // return 1 on success, 0 on error, -1 if unsupported

  virtual int copy(const char *filename, const char *destfilename) { return -1; } // return 1 on success, 0 on error, -1 if unsupported, or it would be just as effecient to read->write.

  virtual void setMetaDataCallback(MetaDataReaderCallback *cb) { }

  virtual int canPrefetch() { return 1; } // return 1 if your reader should prefetch infos about the file in pledit
                                          // (HTTP reader will return 0 here for instance)

protected:
  RECVS_DISPATCH;
};


class NOVTABLE MetaDataReaderCallback : public Dispatchable {
public:
  void metaDataReader_onData(const char *data, int size) { _voidcall(METADATAREADERONDATA,data,size); }

  enum {
    METADATAREADERONDATA=10,
  };
};

// derive from this one
class NOVTABLE MetaDataReaderCallbackI : public MetaDataReaderCallback {
public:
  virtual void metaDataReader_onData(const char *data, int size)=0;

protected:
#undef CBCLASS
#define CBCLASS MetaDataReaderCallbackI
START_DISPATCH;
      VCB(METADATAREADERONDATA, metaDataReader_onData);
END_DISPATCH;
#undef CBCLASS
};

#include <bfc/string/redirstr.h>
#include "../svc_enum.h"

// note: this class handles both redirection
// NOTE: WaI> Unlike aol-wasabi, you have to open the file explicitly, the enum won't do it.
class FileReaderEnum : public SvcEnumT<svc_fileReader> {
public:
  FileReaderEnum(const char *filename, int mode=SvcFileReader::READ, int allow_redirect=FALSE) :
    fn(filename), m(mode)
  {
    if (allow_redirect) {
      fn = RedirString(filename, "Filename");
    }
  }
  virtual int testService(svc_fileReader *svc) {
    return svc->isMine(fn, m);
  }

private:
  String fn;
  int m;
};

#endif
