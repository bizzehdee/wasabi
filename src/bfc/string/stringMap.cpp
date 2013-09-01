#include <precomp.h>
#include "stringMap.h"

stringMap::stringMap() {

}

stringMap::~stringMap() {

}

void stringMap::addPair(stringPair *newPair) {
	smap.addItem(newPair);
}

void stringMap::addPair(const char *s1, const char *s2) {
	smap.addItem(new stringPair(s1, s2));
}

const char *stringMap::getMapped(const char *s) {
	foreach(smap)
		if(!STRICMP(smap.getfor()->getString1(), s)) {
			return smap.getfor()->getString2();
		}
	endfor
	return s;
}

const char *stringMap::enumMapped(int n) {
  return smap.enumItem(n)->getString2();
}

int stringMap::getNumPairs() {
  return smap.getNumItems();
}
