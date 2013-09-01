#include <precomp.h>

#include "xmlwrite.h"

#include <bfc/std.h>
#include <bfc/string/utf8.h>
#include <bfc/string/string.h>
#include <parse/paramparser.h>

#define XMLVERSION "1.0"

#include <api/xml/parser/xmltok_impl.h>

static unsigned char xmltypecheck[256] = {
#define BT_COLON BT_NMSTRT
#include <api/xml/parser/asciitab.h>
#undef BT_COLON
#include <api/xml/parser/latin1tab.h>
};

#define EFPRINTF (nohicharconversion ? fprintf : efprintf)

XMLWrite::XMLWrite(const char *filename, const char *doctype, const char *dtddoctype, int no_hi_chars_conversion, int wantcr) {
  nohicharconversion = no_hi_chars_conversion;
  Init(fopen(filename, "wt"), doctype, dtddoctype, wantcr);
}

XMLWrite::XMLWrite(FILE *file, const char *doctype, const char *dtddoctype, int no_hi_chars_conversion, int wantcr) {
  nohicharconversion = no_hi_chars_conversion;
  Init(file,doctype, dtddoctype, wantcr);
}

void XMLWrite::Init(FILE *file, const char *doctype, const char *dtddoctype, int wantcr) {
  fp = file;
  ASSERT(fp != NULL);   // sheet, need exceptions here
  indent = 0;
  fprintf(fp, "<?xml version=\"" XMLVERSION "\" encoding='UTF-8' standalone=\"yes\"?>%s",wantcr ? "\n": "");
  if (dtddoctype != NULL) fprintf(fp, "<!DOCTYPE %s>\n", dtddoctype);
  if (doctype != NULL) pushCategory(doctype, 1, 0);
}

XMLWrite::~XMLWrite() {
  popCategory(1, 0);
  fflush(fp);
  fclose(fp);
  ASSERT(titles.peek() == 0);
}

void XMLWrite::comment(const char *comment) {
  fprintf(fp, "<!-- %s -->\n", comment);
}

void XMLWrite::pushCategory(const char *title, int wantcr, int wantindent) {
  if (wantindent && indent) // because %*s will put one space event if ident == 0 :/
    fprintf(fp, "%*s<%s>%s", indent*2, " ", title, wantcr ? "\n": "");
  else
    fprintf(fp, "<%s>%s", title, wantcr ? "\n": "");
  indent++;
  ParamParser pp(title, " ");
  titles.push(STRDUP((char *)pp.enumItem(0)));
}

void XMLWrite::pushCategoryAttrib(const char *title, int nodata, int wantindent) {
  if (wantindent && indent)
    fprintf(fp, "%*s<%s", indent*2, " ", title);
  else
    fprintf(fp, "<%s", title);
  indent++;
  titles.push(nodata ? NULL : STRDUP((char *)title));
}

void XMLWrite::writeCategoryAttrib(const char *title, const int val) {
  fprintf(fp, " %s=\"%d\"", title, val);
}

void XMLWrite::writeCategoryAttrib(const char *title, const char *str) {
  if (!str) str = "";
  fprintf(fp, " %s=\"", title);
  EFPRINTF(fp, "%s", str);
  fprintf(fp, "\"");
}

void XMLWrite::closeCategoryAttrib(int wantcr) {
  if (titles.top() == NULL) fprintf(fp, " /");
  fprintf(fp, ">%s", wantcr ? "\n" : "");
}

void XMLWrite::writeAttrib(const char *title, const char *text, int wantcr, int wantindent) {
  if (!text) text = "";
  if (wantindent && indent)
    fprintf(fp, "%*s<%s>", indent*2, " ", title);
  else
    fprintf(fp, "<%s>", title);
  EFPRINTF(fp, "%s", text);
  fprintf(fp, "</%s>%s", title, wantcr ? "\n" : "");
}

void XMLWrite::writeAttrib(const char *title, int val, int wantcr, int wantindent) {
  if (wantindent && indent)
    fprintf(fp, "%*s<%s>%d</%s>%s", indent*2, " ", title, val, title, wantcr ? "\n" : "");
  else
    fprintf(fp, "<%s>%d</%s>%s", title, val, title, wantcr ? "\n" : "");
}

void XMLWrite::writeText(const char *text) {
  if (!text) text = "";
  EFPRINTF(fp, "%s", text);
}

int XMLWrite::popCategory(int wantcr, int wantindent) {
  char *title;
  int r = titles.pop(&title);
  if (!r) return 0;
  indent--;
  if (title != NULL) {
    if (wantindent && indent)
      fprintf(fp, "%*s</%s>%s", indent*2, " ", title, wantcr ? "\n" : "");
    else
      fprintf(fp, "</%s>%s", title, wantcr ? "\n" : "");
    FREE(title);
  }
  return titles.peek();
}

int XMLWrite::efprintf(FILE *fp, const char *format, ...) {
  va_list v;
// http://www.w3.org/TR/REC-xml#syntax
  int bcount=0;
  String outstr;
  va_start(v, format);
  outstr.vsprintf(format, v);
  va_end(v);
  int n = outstr.len();
  for (int i = 0; i < n; i++) {
    unsigned int c = ((unsigned char *)outstr.getValue())[i];
    switch (c) {
      case '<': fputs("&lt;", fp); bcount += 4; break;
      case '>': fputs("&gt;", fp); bcount += 4; break;
      case '&': fputs("&amp;", fp); bcount += 5; break;
      case '\"': fputs("&quot;", fp); bcount += 6; break;
      case '\'': fputs("&apos;", fp); bcount += 6; break;
      default:
        if (xmltypecheck[c] != 0) {
          // If your buffers are already UTF8 encoded, write the data straight to the file
#ifndef UTF8
          if (c >= 127)	{ // ASCII-centric, sorry
            bcount += fprintf(fp, "&#x%02x;", c);
          } else {
            fputc(c, fp);
            bcount++;
          }
#else
          fputc(c, fp);
          bcount++;
#endif
        }
      break;
    }
  }
  return bcount;
}

static void unescape(char *str, char *esc, char replace) {
  ASSERT(str != NULL);
  if (*str == 0) return;	// ignore empty strings
  char *c;
  ASSERT(esc != NULL);
  int slen = STRLEN(esc);
  ASSERT(slen > 0);
  while ((c = STRSTR(str, esc)) != NULL) {
    *c = replace;
    if (c[slen-1]) STRCPY(c+1, c+slen);
    else c[1] = 0;
  }
}

void xmlUnescape(char *str) {
  unescape(str, "&lt;", '<');
  unescape(str, "&gt;", '>');
  unescape(str, "&amp;", '&');
  unescape(str, "&quot;", '\"');
  unescape(str, "&apos;", '\'');
  String dup = str;
  UTF8_to_ASCII(dup, str);
}
