/*
    modified from the implementation in waste
    origional code writen by justen frankel
*/

#ifndef _SHA_H_
#define _SHA_H_

#define SHA_OUTSIZE 20
#define SHA_ROTL(X,n) (((X) << (n)) | ((X) >> (32-(n))))
#define SHUFFLE() E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP

class sha {

public:
  sha();
  void add(unsigned char *data, int datalen);
  void final(unsigned char *out);
  void reset();

private:

  unsigned long H[5];
  unsigned long W[80];
  int lenW;
  unsigned long size[2];
};

#endif//_SHA_H_