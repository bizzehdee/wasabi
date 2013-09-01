#include "precomp.h"

#include "urlparse.h"

UrlParse::UrlParse(const char *urlstring) {
  setDefaultProtocol("http");
  setString(urlstring);
}

UrlParse::UrlParse(const UrlParse &s) {
  setString(s.getString());
}

UrlParse::~UrlParse() { }

void UrlParse::setString(const char *urlstring) {
  protocol = hostname = path = resource = NULL;
  port = 0;
  // parse etc
  String cp(urlstring);
  char *colon = STRCHR(cp, ':');
  if (colon == NULL || colon > cp.v()+4) {	// new default_protocol method
    setProtocol(default_protocol);
  } else {
    String p = cp, tmp(colon+1);
    p.changeChar(':', '\0');
    setProtocol(p);
    cp = tmp;
  }
  // eat slashes
  while (cp.v()[0] == '/') {
    String tmp = cp.v()+1;
    cp = tmp;
  }
  char *firstslash = STRCHR(cp, '/');
  if (firstslash == NULL) {	// host only
    hostname = cp;
    return;
  } else {
    String tmp = firstslash+1;
    *firstslash = '\0';
    hostname = cp;
    cp = tmp;
  }

  colon = STRCHR(hostname, ':');	// reusing
  if (colon != NULL) {
    *colon = '\0';
    port = ATOI(colon+1);
    if (port == 0) port = 80;
  }

  if (!cp.isempty()) {
    char *lastslash;
    for (lastslash = (char*)cp.v(); *lastslash; lastslash++) { }	// go to end
    // back up to last slash or beginning
    for (; lastslash > (char*)cp.v() && *lastslash != '/'; lastslash--) { }

    if (*lastslash == '/') {
      resource = lastslash+1;
      *lastslash = '\0';
      path = cp;
    } else {
      resource = cp;	// no more slashes, just the filename->no path
      // path is already NULL
    }
  }
//  resolveDotDot();
}

void UrlParse::setDefaultProtocol(const char *prot) {
  default_protocol = prot;
}

String UrlParse::getString() const {
  String ret;
  ret.printf("%s://%s", protocol.vs(), hostname.vs());
  if (port != 80 && port != 0) ret.cat(StringPrintf(":%d", port));
  if (!path.isempty()) {
    if (path.firstChar() != '/') ret.cat("/");
    ret.cat(path);
  }
  if (!resource.isempty()) {
    if (resource.firstChar() != '/') ret.cat("/");
    ret.cat(resource);
  }
  return ret;
}

const char *UrlParse::getProtocol() const {
  return protocol;
}

const char *UrlParse::getHostname() const {
  return hostname;
}

const char *UrlParse::getPath() const {
  return path;
}

const char *UrlParse::getResource() const {
  return resource;
}

long UrlParse::getPort() const {
  return port;
}

void UrlParse::setProtocol(const char *p) { protocol = p; }
void UrlParse::setHostname(const char *h) { hostname = h; }
void UrlParse::setPath(const char *p) { path = p; }
void UrlParse::setResource(const char *r) { resource = r; }
void UrlParse::setPort(long p) { port = p; }

void UrlParse::concatRelativeUrl(const char *relative) {
  if (*relative == '/') {	// don't keep any path
    setPath("");
    setResource("");
    String outp = getString();
    outp.cat(relative);
    setString(outp);
  } else {
    setResource("/");
    String outp = getString();
    outp.cat(relative);
    setString(outp);
  }
  // resolve path?
}
