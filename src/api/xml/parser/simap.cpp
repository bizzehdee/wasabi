// s2imap.cpp
#include "simap.h"
SIMap::~SIMap()
{
	int i;

	for ( i = oriNumNodes; i < numNodes; i++)
	{
		delete ary[i].s;
		ary[i].s=NULL;
	}
	
	delete ary;
	ary=NULL;
}

SIMap::SIMap(int numnodes, SInode *s2i)
{	
	ary = new SInode[numnodes]; 
	memcpy (ary, s2i, sizeof(SInode)*numnodes);
	numNodes = numnodes;
	oriNumNodes = numnodes;
}

void SIMap::AddNode(SInode *s2i)
{	
	SInode *ary2;	
	if (s2i== NULL) return ;

	numNodes++;
	ary2 = new SInode[numNodes];
	memcpy(ary2, ary, sizeof(SInode)*(numNodes-1));
	delete ary;
	ary = ary2;
	memcpy(&(ary[numNodes-1]),s2i, sizeof(SInode));
}

char *SIMap::String(int n)
{
	int i;

	if (ary) 
	 for (i = 0; i < numNodes; i++)
	 {
		 if (ary[i].i == n) return ary[i].s;
	 }
	return NULL;
}

void SIMap::Remove(int n)
{
	int i;

	if (ary) 
	{
		for (i = 0; i < numNodes; i++)
		{
			if (ary[i].i == n) break;
		}

		if(i>=numNodes) return;

		numNodes--;

		SInode* ary2 = new SInode[numNodes];
		memcpy(ary2, ary, sizeof(SInode)*(i));
		memcpy(ary2+i, ary+i+1, sizeof(SInode)*(numNodes-i));

		delete ary;
		ary = ary2;
	}
}

int SIMap::IntCanBeNew(char *str)
{
	int	ret;
	SInode sn;

	ret = Int(str);
	if (ret == 0)
	{
		if (nextInt < numNodes) nextInt=numNodes;
		ret = nextInt;
		sn.i = nextInt;
		sn.s = new char[strlen(str)+1];
		strcpy(sn.s, str);

		AddNode(&sn);
		nextInt++;
	}
	return Int(str);
}
int SIMap::Int(char *str)
{
	int i;

	if (ary)
		for(i=0; i<numNodes; i++)
		{
			if (!stricmp(ary[i].s, str)) return ary[i].i;
		}
	return 0;
}