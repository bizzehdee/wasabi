#include "precomp.h"

#include "jnetlib/jnetlib.h"
#include "jnetlib/httpget.h"

#include "urlgrab.h"

#include <bfc/std.h>
#include <bfc/string/redirstr.h>

UrlGrab::UrlGrab(const char *_url, const char *filename, const char *useragent, const char *_referrer, const char *proxy, JNL_AsyncDNS *_dns) : referrer(_referrer), dns(_dns) {
  JNL::open_socketlib();
  ASSERT(_url != NULL);
  url = RedirString(_url, "Url");
  if (dns == NULL) {
    dns = new JNL_AsyncDNS();
    delete_dns = 1;
  } else {
    delete_dns = 0;
  }
  final_url = url;
  httpGetter = new JNL_HTTPGet(dns,16384,const_cast<char*>(proxy));
  connected = 0;
  fp = NULL;
  buffer.setSize(65536+16);

  if (useragent != NULL)
    httpGetter->addheader(StringPrintf("User-Agent:%s", useragent));
  else 
    httpGetter->addheader(StringPrintf("User-Agent:%s build %i (Mozilla)", WASABI_API_APP->main_getVersionString(), WASABI_API_APP->main_getBuildNumber()));

  if (_referrer != NULL && *_referrer != '\0')
    httpGetter->addheader(StringPrintf("Referer: %s", _referrer));

  httpGetter->addheader("Accept:*/*");

  if (filename != NULL) {
    tmpfilename = filename;
    fp = fopen(tmpfilename, "wb");
    if (fp == NULL) {
      DebugString("error opening file '%s'", tmpfilename);
    }
    ASSERT(fp != NULL);
  }
  ready = 0;
  in_pos = 0;
  size = 0;
  max_size = 0;	// 0 means no limit

  reply_code = -1;

  started = Std::getTimeStampMS();
}

UrlGrab::~UrlGrab() {
  if (ready != 1 && !tmpfilename.isempty()) {
    UNLINK(tmpfilename.getValue());
  }
  shutdown();
//  JNL::close_socketlib();	your app needs to call this at shutdown
}

void UrlGrab::addHeader(const char *header) {
  ASSERT(httpGetter);
  httpGetter->addheader(header);
}

int UrlGrab::run() {
  if (ready != 0) return ready;
 // if (!httpGetter) return 1; // GL> Uhh? Useless. Oops :P
  if (!connected) {
    httpGetter->connect(url, 0);
    connected = 1;
    return 0;
  }
  //ASSERT(fp != NULL); GL> //CUT
  int r = httpGetter->run();
  if (r == -1) {
    shutdown();
    ready = -1;
    return -1;
  }
  
  int bytes_read = 0;
  while (httpGetter->bytes_available() > 0) {
    char tmpbuf[4096];
    int bytes = 0;

    // read up to 4k into a temporary buffer.
    bytes = httpGetter->get_bytes(tmpbuf, 4096);

    if (max_size != 0 && size + bytes > max_size) {
      shutdown();
      ready = -1;
      return -1;
    }

    // now our downloaded file is this big
    size += bytes;

    // if our holding buffer isn't large enough to hold the new data, 
    // make it 4x bigger than it needs to be (reallocs get expensive).
    if (buffer.getSize() <= in_pos + bytes) {
       buffer.setSize(in_pos + (bytes * 4));
    }

    // push the bytes to the end of the holding block.
    buffer.setMemory(tmpbuf, bytes, in_pos);

    // advance the buffer position for input.
    in_pos += bytes;

    if (in_pos > 65536 && fp != NULL) {
      // write the current buffer
    	fwrite(buffer, in_pos, 1, fp);
      // and start over.
    	buffer.zeroMemory();
      in_pos = 0;
    }    
  }
  
  if (r == 1) {
    // buffer write first.
    if(fp != NULL) {
      // catch any straggling bytes here.
      fwrite(buffer, in_pos, 1, fp);
    }
    // file close next.
    shutdown();
    ready = 1;
  }
  return ready;
}

int UrlGrab::shutdown() {
  if (httpGetter != NULL) {
    last_error = httpGetter->geterrorstr();
    const char *fu = httpGetter->getredirurl();
    if (fu != NULL) final_url = fu;
    // stash some useful headers (FUCKO: get em all)
    headers.setParam("ETag", httpGetter->getheader("ETag"));
    headers.setParam("Last-Modified", httpGetter->getheader("Last-Modified"));
    // and stash HTTP reply code
    reply_code = httpGetter->getreplycode();
    redirect_code = httpGetter->getredirectcode();
  }
  delete httpGetter; httpGetter = NULL;
  if (delete_dns) delete dns;
  dns = NULL;
  if (fp != NULL) fclose(fp); fp = NULL;
  connected = 0;
  return 1;
}

const char *UrlGrab::getUrl() const {
  return url;
}

const char *UrlGrab::getFinalUrl() const {
  if (httpGetter != NULL) final_url = httpGetter->getredirurl();
  return final_url;
}
//CUT  if (httpGetter == NULL) return final_url;
//CUT  final_url = httpGetter->getredirurl();
//CUT  if (final_url.isempty()) final_url = getUrl();
//CUT  return final_url;
//CUT

const char *UrlGrab::getReferrer() const {
  return referrer;
}

String UrlGrab::getHeader(const char *headername) const {
  return headers.getParam(headername);
}

int UrlGrab::getReplyCode() {
  if (httpGetter) return httpGetter->getreplycode();
  return reply_code;
}

int UrlGrab::getRedirectCode() {
  if (httpGetter) return httpGetter->getredirectcode();
  return redirect_code;
}

const char *UrlGrab::getErrorStr() {
  if (!httpGetter) return last_error;
  return httpGetter->geterrorstr();
}

void *UrlGrab::getBuffer() {
  return buffer.getMemory();
}

stdtimevalf UrlGrab::startedAt() const {
  return started;
}

int UrlGrab::getExpectedSize() const {
  return (httpGetter ? httpGetter->content_length() : 0);
}

void UrlGrab::setMaxSize(int m) {
  max_size = MAX(0, m);
}
