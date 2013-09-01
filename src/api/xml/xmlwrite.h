#ifndef _XML_H
#define _XML_H

#include <bfc/std.h>
#include <bfc/stack.h>

class XMLWrite {
public:
  XMLWrite(const char *filename, const char *doctype=NULL, const char *dtddoctype=NULL, int no_hi_chars_conversion=0, int wantcr=1);
  XMLWrite(FILE *file, const char *doctype=NULL, const char *dtddoctype=NULL, int no_hi_chars_conversion=0, int wantcr=1);
  ~XMLWrite();

  void Init(FILE *file, const char *doctype, const char *dtddoctype, int wantcr=1);

  void comment(const char *comment);

  void pushCategory(const char *title, int wantcr=1, int wantindent=1);

  void pushCategoryAttrib(const char *title, int nodata=FALSE, int wantindent=1);
  void writeCategoryAttrib(const char *title, const int val);
  void writeCategoryAttrib(const char *title, const char *val);
  void closeCategoryAttrib(int wantcr=1);

  void writeAttrib(const char *title, const char *text, int wantcr=1, int wantindent=1);
  void writeAttrib(const char *title, int val, int wantcr=1, int wantindent=1);
  void writeText(const char *text);
  int popCategory(int wantcr=1, int wantindent=1);	// returns nonzero if more categories are open

  static int efprintf(FILE *fp, const char *format, ...);

private:
  FILE *fp;
  int indent;
  Stack<char *>titles;
  int nohicharconversion;
};

void COMEXP xmlUnescape(char *str);

#endif

