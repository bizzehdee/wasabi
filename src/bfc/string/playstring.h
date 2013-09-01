#ifndef _PLAYSTRING_H
#define _PLAYSTRING_H

#include <bfc/common.h>
#include <bfc/string/string.h>

class COMEXP Playstring {
public:
  Playstring(const char *val=NULL);
  Playstring(const Playstring &ps);
  ~Playstring();

  const char *getValue() const { return val; }
  operator const char *() const { return getValue(); }

  void setValue(const char *newval);

  // copy
  Playstring& operator =(const Playstring &ps);

protected:
  void _setValue(const char *newval, int tablenum);

private:
  const char *val;
};

class PlaystringComparator {
public:
  // comparator for sorting
  static int compareItem(Playstring *p1, Playstring* p2) {
    return STRCMP(p1->getValue(), p2->getValue());
  }
};

template <int tablenum=0>
class PlaystringT : public Playstring {
public:
  PlaystringT(const char *newval) {
    val = NULL; setValue(newval);
  }
  PlaystringT(const Playstring &ps) {
    val = NULL; setValue(ps.getValue());
  }
  void setValue(const char *newval) { _setValue(newval, tablenum); }
};

#endif
