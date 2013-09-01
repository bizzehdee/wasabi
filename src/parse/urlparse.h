#ifndef _WASABI_URL_H
#define _WASABI_URL_H

#include <bfc/string/string.h>

class UrlParse {
public:
  UrlParse(const char *urlstring=NULL);
  UrlParse(const UrlParse &s);
  virtual ~UrlParse();

  void setString(const char *urlstring);
  String getString() const;

  void setDefaultProtocol(const char *prot);

  operator String() { return getString(); }

  // http://hostname/path/resource:port
  const char *getProtocol() const;
  const char *getHostname() const;
  const char *getPath() const;
  const char *getResource() const;
  long getPort() const;

  void setProtocol(const char *);
  void setHostname(const char *);
  void setPath(const char *);
  void setResource(const char *);
  void setPort(long port);

  void concatRelativeUrl(const char *relative);

private:
  void resolveDotDot();

  String default_protocol;

  String protocol, hostname, path, resource;
  long port;
};

#endif
