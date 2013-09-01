#ifndef _REDIRSTR_H
#define _REDIRSTR_H

#include <bfc/string/string.h>

class RedirString : public String {
public:
  explicit RedirString(const char *val=NULL, const char *domain="Filename");
};

#endif
