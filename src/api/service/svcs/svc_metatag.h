#ifndef _SVC_METATAG_H
#define _SVC_METATAG_H

//UNDER CONSTRUCTION

#include <bfc/dispatch.h>
#include <api/service/services.h>

class svc_metaTag : public Dispatchable {
public:
  static FOURCC getServiceType() { return WaSvc::METATAG; }

  const char *getName();	// i.e. "ID3v2" or something
  
  int testFile(const char *filename);	// or URL, or any playstring

  const char *enumSupportedTag(int n, int *datatype=NULL);	// see metatag.h

  int getMetaData(const char *tag, char *buf, int buflen, int datatype=0);
  int setMetaData(const char *tag, const char *buf, int buflen, int datatype=0);
};

#endif
