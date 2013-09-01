#ifndef _STRING_MAP_H_WASABI
#define _STRING_MAP_H_WASABI

#include <bfc/string/stringPair.h>
#include <bfc/ptrlist.h>
#include <bfc/foreach.h>
#include <bfc/std.h>

class stringMap {
public:
	stringMap();
	virtual ~stringMap();

	virtual void addPair(stringPair *newPair);
	virtual void addPair(const char *s1, const char *s2);

	virtual const char *getMapped(const char *s);
  virtual const char *enumMapped(int n);

  virtual int getNumPairs();

private:
	PtrList<stringPair> smap;
};

#endif //_STRING_MAP_H_WASABI
