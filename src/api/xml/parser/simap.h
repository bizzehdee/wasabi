// stringmap.h
#ifndef simaph
#define simaph
#ifdef WIN32
#include "windows.h"
#else
#include <stdio.h>
#ifdef WASABI_PLATFORM_DARWIN
#include "/usr/include/string.h"
#else
#include <string.h>
#endif
#define stricmp strcasecmp
#endif
typedef struct tag_sinode
{
	int i;
	char *s;
} SInode;

class SIMap
{
	SInode *ary;
	int	numNodes;
	int nextInt;
	int	oriNumNodes;
public:
	SIMap() {ary = NULL; oriNumNodes = numNodes = 0; nextInt=0;}
	SIMap(int numnodes, SInode *s2i); 
	void AddNode(SInode *s2i);
	~SIMap();
	char	*String(int n);
	int		Int(char *str);
	int		IntCanBeNew(char *);
	void	Remove(int n);
};

#endif
