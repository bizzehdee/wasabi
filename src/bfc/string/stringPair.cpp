#include <precomp.h>
#include "stringPair.h"

stringPair::stringPair(const char *s1, const char *s2) {
	setString1(s1);
	setString2(s2);
}

stringPair::~stringPair() {

}

void stringPair::setString1(const char *s) {
	_s1 = s;
}

void stringPair::setString2(const char *s) {
	_s2 = s;
}

const char *stringPair::getString1() {
	return _s1.v();
}


const char *stringPair::getString2() {
	return _s2.v();
}
