#ifndef _WASABI_STD_RAND_H
#define _WASABI_STD_RAND_H

// this is a much better random than the Std one.
// This one is a Mersenne Twister.
namespace StdRand {
  const int stdRAND_MAX=0xffffffff;
  void srandom(unsigned int key=0);//if key==0, uses time()
  unsigned int randomu32(unsigned int max = stdRAND_MAX);  // full 32-bits of randomness
  double random1f();	// 0.0..1.0
};

#endif
