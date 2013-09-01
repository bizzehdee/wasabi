#ifndef WASABI_HISTO_H
#define WASABI_HISTO_H

/*
  Histogram: counts instances of unique color values in a range of ARGB32 data.
  Ignores alpha channel for counting purposes (also skips 0 alpha pixels)
*/

// 10-may-2004 BU created
// 14-jul-2004 BU rewrote, superfaster, and not as wrong

#include <bfc/std.h>
#include <draw/argb32.h>

class Histogram {
public:
  Histogram(ARGB32 *bits, int w, int h, int stride=-1, ARGB32 mask=0xffffffff);
  ~Histogram();

  int getNumColors();
//not implemented  ARGB32 getByPopularity(int n, int *num_occurrences=NULL);
  int getCoverage(ARGB32 pixel);

private:
  ARGB32 mask;
  int num_colors;

  struct Node {
  public:
    Node();
    ~Node();

    Node *getSubNode(unsigned int pos, int create=FALSE);

    int count;
    Node **subnodes;
  };
  Node top;
};

#endif
