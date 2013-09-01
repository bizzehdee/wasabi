#include "precomp.h"

#include <bfc/util/profiler.h>

#include "histo.h"

// 10-may-2004 BU created

Histogram::Histogram(ARGB32 *bits, int w, int h, int stride, ARGB32 mask) {
mask = ~0;//FUCKO
  ASSERT(bits != NULL);
  ASSERT(w >= 1);
  ASSERT(h >= 1);
  num_colors = 0;
  ASSERTPR(stride != 0, "stride cannot be 0!");
  stride = stride < 0 ? w : stride;
  
  //DebugString("enter histo %d x %d", w, h);
//PR_ENTER("histo");
  for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
      ARGB32 pixel = bits[y*stride+x];
      if (ALP(pixel) <= 1) continue;
      pixel &= mask;
      unsigned int r = RED(pixel), g = GRN(pixel), b = BLU(pixel);
      Node *n = top.getSubNode(r, TRUE);
      n = n->getSubNode(g, TRUE);
      n = n->getSubNode(b, TRUE);
      if (n->count++ == 0) num_colors++;
    }
//CUT    Std::sleep(0);
  }
//PR_LEAVE();

//  DebugString("&&& found %d colors", num_colors);
}

Histogram::~Histogram() { }

int Histogram::getNumColors() {
  return num_colors;
}

#if 0// finish me
ARGB32 Histogram::getByPopularity(int n, int *num_occurrences) {
#if 0
  HistRec *ret = plist.enumItem(n);
  ASSERT(ret != NULL);
  if (num_occurrences != NULL) *num_occurrences = ret->b;
  return ret->a;
#endif
ASSERTALWAYS("hi");
return 0;
}
#endif

int Histogram::getCoverage(ARGB32 pixel) {
  int ret = 0;
  pixel &= mask;
  unsigned int r = RED(pixel), g = GRN(pixel), b = BLU(pixel);
  Node *n = top.getSubNode(r);
  if (n == NULL) return 0;
  n = n->getSubNode(g);
  if (n == NULL) return 0;
  n = n->getSubNode(b);
  if (n == NULL) return 0;
  return n->count;
}

Histogram::Node::Node() : count(0), subnodes(NULL) { }

Histogram::Node::~Node() {
  if (subnodes) {
    for (int i = 0; i <= 255; i++) delete subnodes[i];
    FREE(subnodes);
  }
}

Histogram::Node *Histogram::Node::getSubNode(unsigned int pos, int create) {
  if (subnodes == NULL) {
    if (!create) return NULL;
    subnodes = (Node**)CALLOC(sizeof(Node*), 256);
  }
//  ASSERT(pos >= 0);
//  ASSERT(pos <= 255);
  if (subnodes[pos] == NULL && create) subnodes[pos] = new Node;
  return subnodes[pos];
}
