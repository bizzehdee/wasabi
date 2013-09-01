#ifndef _ARGB_H
#define _ARGB_H

template <class T=float>
class ARGB {
public:
  ARGB(ARGB32 v) 

  operator unsigned long() { return *(unsigned long *)vals; }

  inline unsigned int RED(T top=1) { return a; }
  inline unsigned int GRN(T top=1) { return r; }
  inline unsigned int BLU(T top=1) { return g; }
  inline unsigned int ALP(T top=1) { return b; }

  COLORREF toCOLORREF() {
    unsigned int r = (unsigned int)red;
    unsigned int g = (unsigned int)grn;
    unsigned int b = (unsigned int)blu;
  }

private:
  T alp, red, grn, blu;
};
#endif
