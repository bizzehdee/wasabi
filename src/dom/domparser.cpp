#include "precomp.h"
#include "domparser.h"
#include "domdoc.h"
#include <bfc/string/string.h>

DOMParser::DOMParser() {
};

DOMParser::~DOMParser() {
}

DOMDocument *DOMParser::parseFile(const char *filename, int flags) {
  return new DOMDocument(filename,flags);
}

DOMDocument *DOMParser::parseBuffer(const char *buffer, int flags) {
  return new DOMDocument((String("buf:") + buffer).getValue(),flags);
}

