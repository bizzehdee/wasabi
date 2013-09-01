#ifndef _TMPNAMESTR_H
#define _TMPNAMESTR_H

#include <bfc/string/string.h>

class TmpNameStr : public String {
public:
  TmpNameStr(const char *prefix="") {	// prefix can be 1-3 characters
    valid = 0;
    make(prefix);
  }
  TmpNameStr(const char *file, int line) {
    make("dtn");	// dtn -> debug tmpname
    FILE *fp = FOPEN(getValue(), "wt");
    FPRINTF(fp, "%s @ %d", file, line);
    FCLOSE(fp);
  }

  int isValid() {	// always check this! tmpnames can run out
    return valid;
  }

private:
  int make(const char *prefix="") {
    char tmp[WA_MAX_PATH]="";
    valid = TMPNAM(tmp, prefix);
    if (valid) setValue(tmp);
    else setValue("");	// impossible filename on most OSs
    return valid;
  }
  int valid;
};

// for debugging, writes the calling filename and line number into the file
#define DEBUGTMPNAMESTR(obj) TmpNameStr obj(__FILE__, __LINE__)

#endif
