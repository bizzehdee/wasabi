#ifndef __API_FILE_IMPL_H
#define __API_FILE_IMPL_H

/*<?<autoheader/>*/
#include "api_filereader.h"
#include "api_filereaderx.h"
/*?>*/
/*[interface.header.h]
namespace FileReader {
  enum {
    READ=1,
    WRITE=2,
    APPEND=4,
    PLUS=8,
    BINARY=16,
    TEXT=32,
  };
};
*/

#include <api/service/services.h>
#include <api/service/svcs/svc_fileread.h>

/*<?<classdecl name="api_filereader" implname="api_filereaderI" factory="Interface" dispatchable="1"/>*/
class api_filereaderI : public api_filereaderX {
/*?>*/
public:
  NODISPATCH api_filereaderI();
  NODISPATCH virtual ~api_filereaderI();
  
  static const char *getServiceName() { return "Filereader Api"; }
  static GUID getServiceType() { return WaSvc::UNIQUE; }

  DISPATCH(10) int fileCanOpen(const char *filename, int mode);

  DISPATCH(20) void *fileOpen(const char *filename, int mode);
  DISPATCH(30) void fileClose(void *file);
  DISPATCH(40) int fileRead(void *file, char *buf, int len);
  DISPATCH(50) int fileWrite(void *file, char *buf, int len);

  DISPATCH(60) int fileCanSeek(void *file);
  DISPATCH(70) int fileSeek(void *file, int start, int dist);

  DISPATCH(80) int fileExists(const char *filename);
  DISPATCH(90) int fileIsDirectory(const char *filename);
};

/*[interface.footer.h]
// {9879C23B-0DD1-4494-AFA2-DAF9CC1E4F60}
static const GUID fileApiServiceGuid= 
{ 0x9879c23b, 0xdd1, 0x4494, { 0xaf, 0xa2, 0xda, 0xf9, 0xcc, 0x1e, 0x4f, 0x60 } };

extern api_filereader *fileApi;
*/

#endif // __API_FILE_IMPL_H
