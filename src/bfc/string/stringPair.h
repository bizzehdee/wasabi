#ifndef _STRING_PAIR_H_WASABI
#define _STRING_PAIR_H_WASABI

#include <bfc/string/string.h>

class stringPair {
public:
	stringPair(const char *s1, const char *s2);
	virtual ~stringPair();

	void setString1(const char *s);
	void setString2(const char *s);

	const char *getString1();
	const char *getString2();

private:
	String _s1;
	String _s2;
};

#endif //_STRING_PAIR_H_WASABI
