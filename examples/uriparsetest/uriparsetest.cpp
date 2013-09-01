/**
 * Tests UriParse
 *
 * compile using wasabi configured for standalonebfc.
 *
 */
#include <parse/uriparse.h>
#include <stdio.h>

int TestUri(const char *base, const char *relative=NULL) {
  printf("Testing URI: <%s>", base);
  if(relative) printf(" + <%s>", relative);
  printf("\n");

  UriParse uri(base, relative);
  //*/
  printf("scheme:    %s\n", uri.getScheme()    ? uri.getScheme():"(null)");
  printf("authority: %s\n", uri.getAuthority() ? uri.getAuthority():"(null)");
  printf("path:      %s\n", uri.getPath()      ? uri.getPath():"(null)");
  printf("query:     %s\n", uri.getQuery()     ? uri.getQuery():"(null)");
  printf("fragment:  %s\n", uri.getFragment()  ? uri.getFragment():"(null)");
  /**/
  printf("canonical: <%s>\n\n", uri.getCanonicalString().vs());

}

int main(int argc, const char *argv[]) {

  //a few easy ones:
  TestUri("http://www.w3.org/path/to/script.php?a=3%32;b=90#bestpart");
  TestUri("mailto:postmaster@w3.org");
  TestUri("http://foo.com/a/b/c/d#g", "f");
  TestUri("http://foo.com/a/b/c/d#g", "?q=0");
  TestUri("http://foo.com/a/b/c/d#g", "../../f");
  TestUri("http://foo.com/a/b/c/d#g", ".././f");
  TestUri("http://foo.com/a/b/c/d#g", "../../f#h");
  TestUri("http://foo.com/a/b/c/d#g", "../../f?q=1");
  TestUri("http://foo.com/a/b/c/d#g", "../../../../../f");
  //some examples from the RFC
  TestUri("http://a/b/c/d;p?q", "g:h");
  TestUri("http://a/b/c/d;p?q", "g");
  TestUri("http://a/b/c/d;p?q", "./g");
  TestUri("http://a/b/c/d;p?q", "g/");
  TestUri("http://a/b/c/d;p?q", "/g");
  TestUri("http://a/b/c/d;p?q", "//g");
  TestUri("http://a/b/c/d;p?q", "?y");
  TestUri("http://a/b/c/d;p?q", "g?y");
  TestUri("http://a/b/c/d;p?q", "#s");
  TestUri("http://a/b/c/d;p?q", "g#s");
  TestUri("http://a/b/c/d;p?q", "g?y#s");
  TestUri("http://a/b/c/d;p?q", ";x");
  TestUri("http://a/b/c/d;p?q", "g;x");
  TestUri("http://a/b/c/d;p?q", "g;x?y#s");
  TestUri("http://a/b/c/d;p?q", "");
  TestUri("http://a/b/c/d;p?q", ".");
  TestUri("http://a/b/c/d;p?q", "./");
  TestUri("http://a/b/c/d;p?q", "..");
  TestUri("http://a/b/c/d;p?q", "../");
  TestUri("http://a/b/c/d;p?q", "../g");
  TestUri("http://a/b/c/d;p?q", "../..");
  TestUri("http://a/b/c/d;p?q", "../../");
  TestUri("http://a/b/c/d;p?q", "../../g");
  //File URI's suck:
  TestUri("file:///home/user/.rc");
  TestUri("file:/home/user/.rc");
  TestUri("file:///c/Windows/");
  TestUri("file://c/Windows/"); // this is an incorrect URI
  TestUri("file:/c/Windows/");
  TestUri("file:c:/Windows/");
  //Test pctEncode:
  TestUri("http://%80%95%AB%FE/");

  return 0;
}
