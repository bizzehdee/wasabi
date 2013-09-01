#ifndef __BIGSTRING_H
#define __BIGSTRING_H

/*

A class tor concatenate chunks of texts into one big pool. This is much faster than using String if you are adding
a lot of tiny pieces into one giant block (a typical cases of this is when writing xml). Upon request for the value,
the class allocates one big block of memory and copies all the strings into it serially (as opposed to String
reallocating the entire block at each concatenation). Note that because of the type of implementation BigString has
to use, you cannot get the full block as a return value to your concatenations and assignments (+= and = return void).
{{{ BU NOTE: you can use the mutable keyword to solve this problem }}}
To do this, request the value explicitely (this should be kept to a strict minimum or the advantage of BigString over
String will disapear)

*/

#include <bfc/string/string.h>
#include <bfc/ptrlist.h>

class BigString {
  public:
    BigString(const char *initial_val = NULL);
    virtual ~BigString();

    operator const char *() /*const*/ { return getValue(); }
    const char *getValue() /*const*/;
    const char *getValueSafe(const char *def_val="") /*const*/;
    const char *v() { return getValue(); }
    const char *vs(const char *def_val="") { return getValueSafe(def_val); }
    int len();
    char *getNonConstVal() { return (char *)getValue(); }
    void setValue(const char *val);
    // copy assignment operator
    BigString& operator =(/*const*/ BigString &s) {
      if (this != &s)
        setValue(s);
      return *this;
    }

    void operator =(const char *newval) { setValue(newval); }
    void operator +=(const char *addval) {
      cat(addval);
    }

    int isempty();
    inline int notempty() { return !isempty(); }

    void reset();

    void catn(const char *s, int n);
    void cat(const char *s);
    char lastChar();
    char firstChar();
    int getLineCount();

    // this only works on the individual lines. anything that crosses
    // a line boundary will not be replaced
    int replace(const char *find, const char *replace);

  private:
    PtrList<String> strings;
    char *mem;
    int memsize;
    int m_linecount;
};

#endif
