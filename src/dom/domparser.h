#ifndef DOMPARSER_H
#define DOMPARSER_H

class DOMDocument;

class DOMParser {
public:
	DOMParser();
	virtual ~DOMParser();

	DOMDocument *parseFile(const char *filename, int flags=0);
	DOMDocument *parseBuffer(const char *buffer, int flags=0);
};

#endif

