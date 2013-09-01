#include <precomp.h>
#include "url.h"
#include "bigstring.h"

#include <bfc/std.h>

void Url::encode(String &_dest, int use_plus_for_space, int encoding, int style) {
  if (_dest.isempty()) return;
  String srcstr = _dest;
  const char *src = srcstr;
  BigString dest;
  if (encoding & URLENCODE_EXCLUDEHTTPPREFIX)
    if (!STRNINCMP(src, "http://")) {
      src += STRLEN("http://");
      dest += "http://";
    }
    else if (!STRNINCMP(src, "https://")) {
      src += STRLEN("https://");
      dest += "https://";
    }
  while (*src) {
    int encode = 1;
    if (encoding & URLENCODE_NOTHING) encode = 0;
    if ((encoding & URLENCODE_EXCLUDEALPHANUM) && (ISALPHA(*src) || ISDIGIT(*src) || *src == '_' || *src == '-' || *src == '.') && (unsigned char)*src < 128) encode = 0;
    if ((encoding & URLENCODE_EXCLUDE_8BIT) && ((unsigned char)*src > 127)) encode = 0;
    if ((encoding & URLENCODE_EXCLUDE_7BIT_ABOVEEQ32) && ((unsigned char)*src >= 32 && (unsigned char)*src <= 127)) encode = 0;
    if ((encoding & URLENCODE_ENCODESPACE) && *src == ' ') encode = 1;
    if ((encoding & URLENCODE_ENCODEXML) && (*src == '<' || *src == '>' || *src == '&')) encode = 1;
    if ((*src == '&' && (style == URLENCODE_STYLE_ANDPOUND || style == URLENCODE_STYLE_ANDPOUNDX)) || 
        (*src == '%' && style == URLENCODE_STYLE_PERCENT)) encode = 1;
    if ((encoding & URLENCODE_EXCLUDESLASH) && *src == '/') encode = 0;
    if (!encode) {
      dest.catn(src, 1);
    } else if (use_plus_for_space && *src == ' ') {
      dest += "+";
    } else {
      switch (style) {
        case URLENCODE_STYLE_PERCENT:
          dest += StringPrintf("%%%02X", (int)*(unsigned char *)src);
          break;
        case URLENCODE_STYLE_ANDPOUND:
          dest += StringPrintf("&#%02d;", (int)*(unsigned char *)src);
          break;
        case URLENCODE_STYLE_ANDPOUNDX:
          dest += StringPrintf("&#x%02X;", (int)*(unsigned char *)src);
          break;
      }
    }
    src++;
  }
  _dest = dest;
}

void Url::decode(String &str, int use_plus_for_space) {
  if (str.isempty()) return;
  char *s = (char *)MALLOC(STRLEN(str)+1);
  char *p = str.getNonConstVal();
  char *d = s;

  while (*p) {
    if (*p == '%') {
      int abort = 0;
      if (!*(p+1)) { *d++ = *p++; break; }
      if (*(p+1) == '%') {
        *d = *p;
        p = p+2;
        d++;
      }
      int a = *(p+1);
      int b = 0;
      if (a != 0) b = *(p+2);
      a = TOUPPER(a);
      b = TOUPPER(b);
      if (a >='0' && a <= '9') a -= '0';
      else if (a >= 'A' && a <= 'F') { a -= 'A'; a += 10; }
      else abort = 1;
      if (b >='0' && b <= '9') b -= '0';
      else if (b >= 'A' && b <= 'F') { b -= 'A'; b += 10; }
      else abort = 1;
      if (abort) {
        *d++ = *p++;
        continue;
      }
      *d++ = a*16+b;
      p = p+3;
      continue;
    }
    *d = *p;
    p++; d++;
  }
  *d = 0;

  str = s;
  FREE(s);
}
