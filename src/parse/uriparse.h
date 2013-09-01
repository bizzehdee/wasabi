#ifndef _WASABI_URI_H
#define _WASABI_URI_H

#include <bfc/string/string.h>

// This code is written to comply with RFC 3986: Uniform Resource Identifier (URI): Generic Syntax
// see: http://www.gbiv.com/protocols/uri/rfc/rfc3986.html
// It is not ment to provide perfect validation of URIs, but should be able to parse and
// percent-decode all valid URIs as well as resolve relative URIs.

#define URI_SUBDELIMS "!$&'()*+,;="
#define URI_GENDELIMS ":/?#[]@"

class UriParse {
public:
  UriParse(const char *baseuri=NULL, const char *relativeuri=NULL) { parse(baseuri,relativeuri); }
  UriParse(const UriParse &base, const char *relativeuri) { parseRelativeUri(base,relativeuri); }
  UriParse(const UriParse &cpy);
  virtual ~UriParse() {}

  /**
    Parse a URI reference with optional relative URI.
  */
  int parse(const char *baseuri, const char *relativeuri=NULL) { return _parse(baseuri, relativeuri); }
  /**
    Parse a URI reference relative to a base URI.
  */
  int parseRelativeUri(const UriParse &base, const char *relativeuri=NULL);
  /**
    Return the URI in it's canonical form, including percent-encoding reserved characters.
  */
  String getCanonicalString() const;

  //these don't use vs() for easy-detection of NULL parts
  const char *getScheme() const { return scheme.v(); }
  const char *getAuthority() const { return authority.v(); }
  const char *getPath() const { return path.v(); }
  const char *getQuery() const { return query.v(); }
  const char *getFragment() const { return fragment.v(); }

  void setScheme(const char *s) { scheme = s; }
  void setAuthority(const char *a) { authority = a; }
  void setPath(const char *p) { path = p; }
  void setQuery(const char *q) { query = q; }
  void setFragment(const char *f) { fragment = f; }

  //utilities used in this class, but you might find them useful too:

  /**
    Decodes percent-encoded octets in str. Ignores invalid encodings (e.g. "%z6" will remain "%z6").
    Decodes in-place since the decoded string will always be less-than or equal in length.
    @return The count of decoded octets.
  */
  static int pctDecode(String &str);
  /**
    percent-encodes all characters except unreserved ( ALPHA / DIGIT / "-" / "." / "_" / "~" ) and those in noencode.
  */
  static String pctEncode(const String &str, const char *noencode);

  // utility functions for relative URI parsing as described by the RFC.
  static String UriParse::merge_paths(const UriParse &base, const UriParse &relative);
  static String UriParse::remove_dots(const String &path);

private:
  //http://foo.com/path/file.php?a=32#bar
  String scheme;    // "http"
  String authority; // "foo.com"
  String path;      // "/path/file.php"
  String query;     // "a=32"
  String fragment;  // "bar"

  int _parse(const char *baseuri, const char *relativeuri=NULL, int empty_scheme_ok=FALSE);

};

#endif
