#ifndef _WASABI_URLGRAB_H
#define _WASABI_URLGRAB_H

#include <stdio.h>
#include <bfc/string/string.h>
#include <bfc/memblock.h>
#include <bfc/paramobj.h>

class JNL_HTTPGet;
class JNL_AsyncDNS;

/* usage:
  #define TMPPREFIX "abc" // change for your app ;)
  const char *useragent="Mozilla/4.0 (compatible; WasabiApp/Win32)"
  UrlGrab *url_grabo = new UrlGrab(url, TmpNameStr(TMPPREFIX), useragent);
  while (1) {
    int r = url_grabo->run();
    if (r == 0) {
      Std::usleep(100);	// or whatever, doesn't need to be too often
      continue;
    }
    if (r < 0) {
      DebugString("url grab failed:", url_grabo->getErrorStr());
    } else {
      String filename = url_grabo->getFilename();
      // you take it from here!
    }
  };
// end usage example */
  
class UrlGrab {
public:
  UrlGrab(const char *url, const char *filename=NULL, const char *useragent=NULL, const char *referrer=NULL, const char *proxy=NULL, JNL_AsyncDNS *dns=NULL);
  ~UrlGrab();

public:
  void addHeader(const char *header);	// call before first run()

  int run();	// returns 1 when file is ready, 0 = wait, -1 = error
  int shutdown();

  int filesize() const { return size; }
  int getExpectedSize() const;

  // forces error if > this many bytes are downloaded (optional)
  void setMaxSize(int m);

  const char *getFilename() const { return tmpfilename; }

  // this might not be what was originally passed in due to Url redirects
  // (as in svc_redir, not http redirecting, although that will work
  // transparently too)
  const char *getUrl() const;
  // this one will return the post-http-redirect url that we ended up at
  const char *getFinalUrl() const;

  const char *getReferrer() const;	// whatever was in the cons

  // this only caches and returns "ETag" and "Last-Modified" for now
  String getHeader(const char *headername) const;

  int getReplyCode();
  int getRedirectCode();

  const char *getErrorStr();

  void *getBuffer();

  stdtimevalf startedAt() const;

private:
  MemBlock<char> buffer;

  JNL_HTTPGet *httpGetter;
  JNL_AsyncDNS *dns;
  int delete_dns;	// if 1, delete dns on destruct
  FILE *fp;
  String url, tmpfilename;
  mutable String final_url;
  String referrer;
  String last_error;
  ParamObj headers;
  int reply_code;
  int redirect_code;	// either 0, 301, or 302
  int connected, ready;
  int size;   // the total size read.
  int max_size; // optional limit on download size
  int in_pos; // the position in the buffer into which to read from the socket.
  stdtimevalms started;
};

#endif
