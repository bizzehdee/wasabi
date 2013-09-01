#include "uriparse.h"
#include <bfc/assert.h>

UriParse::UriParse(const UriParse &cpy) {
  scheme = cpy.scheme;
  authority = cpy.authority;
  path = cpy.path;
  query = cpy.query;
  fragment = cpy.fragment;
}

int UriParse::_parse(const char *baseuri, const char *relativeuri, int empty_scheme_ok) {

  scheme.purge();
  authority.purge();
  path.purge();
  query.purge();
  fragment.purge();

  if(!baseuri) return 0;

  String base = baseuri;

  // parse scheme:

  int colon = base.lFindChar(':');
  //scheme = base.lSliceChar(':');

  //validate scheme:
  if(colon > 0 /*scheme.notempty()*/) {
    scheme = base.lSplit(colon);
    scheme.setChar(colon, '\0');
    scheme.tolower(); //normalize
    if(!ISALPHA(scheme.getChar(0))) { scheme.purge(); return 0; } //invalid URI
    int l = scheme.len();
    for(int i = 1; i < l; i++) {
      char c = scheme.getChar(i);
      if(ISALPHA(c)) continue;
      if(ISDIGIT(c)) continue;
      if(c == '+' || c == '-' || c == '.') continue;
      scheme.purge(); return 0; //invalid URI
    }
  } else {
    if(!empty_scheme_ok) return 0;
  }

  //authority and path, or just path?
  if(base.len() >= 2 && base.getChar(0) == '/' && base.getChar(1) == '/') {
    //parse authority
    base.lSplit(1); //remove "//"
    int authend = base.lFindChar('/');
    if(authend < 0) {
      authority = base;
      pctDecode(authority);
      return parseRelativeUri(*this, relativeuri); //valid URI w/o path. parse relativeuri.
    } else if(authend != 0) {
      authority = base.lSplit( authend - 1 ); //ends at beginning of path
      pctDecode(authority);
    }

#if defined(WASABI_PLATFORM_WIN32)
    //detect file scheme peculiarity with "c:" etc, appearing as an authority
    //note this actually violates the spec, but we'll accept it anyway, since most
    //Windows apps do.
    if(scheme == "file") {
      if(authority.len() == 2 && ISALPHA(authority.getChar(0)) && authority.getChar(1) == ':') {
        base.prepend(authority); //put it back to be parsed into the path.
        authority.purge();
      }
    }
#endif
  }

  //parse optional path. path includes any leading '/'.
  int pathend = base.lFindChar("?#");
  if(pathend < 0) {
    path = base; //(note this could potentially be empty)
    pctDecode(path);
    return parseRelativeUri(*this, relativeuri); //valid URI w/o query or fragment. parse relativeuri.
  }
  else if(pathend != 0) { //don't parse an empty path
    path = base.lSplit( pathend - 1 );
    pctDecode(path);
  }

  //parse optional query
  if(base.len() > 0 && base.getChar(0) == '?') {
    base.lSplit(0); //remove "?"
    int queryend = base.lFindChar('#');
    if(queryend == -1) {
      query = base;
      pctDecode(query);
      return parseRelativeUri(*this, relativeuri); //valid URI w/o fragment. parse relativeuri.
    }
    query = base.lSplit( queryend - 1 ); // ends at beginning of fragment
    pctDecode(query);
  }

  //parse optional fragment
  if(base.len() > 0 && base.getChar(0) == '#') {
    base.lSplit(0); //remove "#"
    fragment = base;
    pctDecode(fragment);
    return parseRelativeUri(*this, relativeuri); //valid URI. parse relativeuri.
  }

  return parseRelativeUri(*this, relativeuri);

}

int UriParse::parseRelativeUri(const UriParse &base, const char *relativeuri) {
  if(relativeuri == NULL) return 1;

  //parse relativeuri into it's parts. an empty scheme is ok, even expected.
  UriParse relative;
  relative._parse(relativeuri, NULL, TRUE);

  if(relative.scheme == base.scheme) {
    relative.scheme.purge();
  }

  if(relative.scheme.notempty()) {
    scheme = relative.scheme;
    authority = relative.authority;
    path = relative.path;
    query = relative.query;
  } else {
    if(relative.authority.notempty()) {
      authority = relative.authority;
      path = remove_dots(relative.path);
      query = relative.query;
    } else {
      if(relative.path.isempty()) {
        path = base.path;
        if(relative.query.notempty())
          query = relative.query;
        else
          query = base.query;
      } else {
        if(relative.path.getChar(0) == '/') {
          path = remove_dots(relative.path);
        } else {
          path = merge_paths(base, relative);
          path = remove_dots(path);
        }
        query = relative.query;
      }
      authority = base.authority;
    }
    scheme = base.scheme;
  }

  fragment = relative.fragment;
}

String UriParse::getCanonicalString() const {

  String ret;

  if(scheme.isempty()) return ret; //invalid URI

  ret += scheme;
  ret += ':';
  if(authority.notempty() || scheme == "file") {
    ret += "//";
    //endcode non-( '@' / ':' / unreserved / sub-delims )
    ret += pctEncode(authority, "@:[]" URI_SUBDELIMS);
  }

#if defined(WASABI_PLATFORM_WIN32)
  //if this is a path from a drive-letter root, append the leading '/'
  if(scheme == "file") {
    if(path.len() >= 2 && ISALPHA(path.getChar(0)) &&
      ( (path.len() >= 3 && path.getChar(1) == ':' && path.getChar(2) == '/') ||
        path.getChar(1) == '/'  )
      ) {
      ret += '/';
    }
  }
#endif

  if(authority.notempty() && ((path.notempty() && path.getChar(0) != '/') || path.isempty())) {
    // make sure path-abempty begins with a '/'
    ret += '/';
  }

  //path is required, but can be empty
  //encode non-( pchar / '/' )
  ret += pctEncode(path, "/@:" URI_SUBDELIMS);

  if(query.notempty()) {
    ret += '?';
    //encode non-( pchar / "/" / "?" )
    ret += pctEncode(query, "/?@:" URI_SUBDELIMS);
  }

  if(fragment.notempty()) {
    ret += '#';
    //encode non-( pchar / "/" / "?" )
    ret += pctEncode(fragment, "/?:" URI_SUBDELIMS);
  }

  return ret;
}

int UriParse::pctDecode(String &str) {
  int o = 0, count = 0;
  char c, buf[3];
  unsigned int h;
  int n = str.len();
  for(int i = 0; i < n; ++i) {
    if( (c=str.getChar(i)) == '%' && i < n-2) {
      buf[0] = str.getChar(i+1);
      buf[1] = str.getChar(i+2);
      buf[2] = '\0';
      if(sscanf(buf, "%2x", &h) < 1 || h == '\0') {
        //bad percent-encoding. we'll ignore it, and output the '%'
        str.setChar(o++, '%');
      } else {
        str.setChar(o++, h);
        i+=2;
        ++count;
      }
    } else {
      str.setChar(o++, c);
    }
  }
  str.setChar(o++, '\0');

  return count;
}

String UriParse::pctEncode(const String &str, const char *noencode) {
  String ret;
  int len = str.len();
  int noenclen = strlen(noencode);
  int doit = 0;
  unsigned int c;
  for(int i = 0; i < len; ++i) {
    c = str.getChar(i);
    if(ISALPHA(c));
    else if(ISDIGIT(c));
    else if(c == '-' || c == '.' || c == '_' || c == '~');
    else {
      int j;
      for(j = 0; j < noenclen; ++j) {
        if(c == noencode[j]) break;
      }
      if(j == noenclen) doit = TRUE;
    }
    if( doit ) {
      c &= 0xff;
      ret += StringPrintf("%%%02X", c);
      doit == FALSE;
    } else {
      ret += (char)c;
    }
  }
  return ret;
}

String UriParse::merge_paths(const UriParse &base, const UriParse &relative) {
  /* (from the RFC, Section 5.2.3
    * If the base URI has a defined authority component and an empty path, then return a string
      consisting of "/" concatenated with the reference's path; otherwise,
    * return a string consisting of the reference's path component appended to all but the last
      segment of the base URI's path (i.e., excluding any characters after the right-most "/"
      in the base URI path, or excluding the entire base URI path if it does not contain any "/" characters).
  */
  if(base.authority.notempty() && base.path.isempty())
    return StringPrintf("/%s", relative.path.vs());

  int lastslash = base.path.rFindChar('/');

  if(lastslash < 0)
    return relative.path;

  return String(base.path).lSplit(lastslash) + relative.path;

}

String UriParse::remove_dots(const String &path) {

  int pathlen = path.len();
  char *inbuf = (char *)MALLOC(pathlen+1);
  path.strncpyTo(inbuf,pathlen+1);
  char *in = inbuf;

  char *outbuf = (char *)MALLOC(pathlen+1); // output can't be longer than input
  char *out = outbuf;
  *out = '\0';

  while(*in != '\0') {
    /* A. If the input buffer begins with a prefix of "../" or "./", then remove that prefix from the input buffer; */
    if(STRNNCMP(in, "../") == 0) in += 3;
    else if(STRNNCMP(in, "./") == 0) in += 2;

    /* B. if the input buffer begins with a prefix of "/./" or "/.", where "." is a complete path segment,
          then replace that prefix with "/" in the input buffer; */
    else if(STRNNCMP(in, "/./") == 0) in += 2;
    else if(STRCMP(in, "/.") == 0) { /* as far as I can tell, 'where "." is a complete path segment' is the same as '"/." is the rest of the input buffer' */
      ++in;
      *in = '/';
    }

    /* C. if the input buffer begins with a prefix of "/../" or "/..", where ".." is a complete path segment,
          then replace that prefix with "/" in the input buffer and remove the last segment and its
          preceding "/" (if any) from the output buffer; */
    else if(STRNNCMP(in, "/../") == 0) {
      in +=3;
      while((out-1) >= outbuf && *(out-1) != '/') --out;
      if(out != outbuf && *(out-1) == '/') --out;
    }
    else if(STRCMP(in, "/..") == 0) { // as in B.
      in += 2;
      *in = '/';
      while((out-1) >= outbuf && *(out-1) != '/') --out;
      if(out != outbuf && *(out-1) == '/') --out;
    }

    /* D. if the input buffer consists only of "." or "..", then remove that from the input buffer; */
    else if(STRCMP(in, ".") == 0) in += 1;
    else if(STRCMP(in, "..") == 0) in += 2;

    /* E. move the first path segment in the input buffer to the end of the output buffer, including the
          initial "/" character (if any) and any subsequent characters up to, but not including, the
          next "/" character or the end of the input buffer. */
    else {
      if(*in && *in == '/') *out++ = *in++;  // initial "/" (if any)
      while(*in && *in != '/') *out++ = *in++; // subsequent chars up to and excluding next "/" or end of inbuf.
    }
  }

  *out = '\0';

  ASSERT( (out-outbuf) < (pathlen + 1) ); // test my assumption that output can't be longer than input.

  FREE(inbuf);
  String ret(outbuf);
  FREE(outbuf);

  return ret;
}
