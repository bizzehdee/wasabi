#ifndef _PAIR_H
#define _PAIR_H

template <class A, class B>
class Pair {
public:
  Pair(A _a, B _b) : a(_a), b(_b) {}

  A a;
  B b;
};

template <class A, class B, class C>
class Triple {
public:
  Triple(A _a, B _b, C _c) : a(_a), b(_b), c(_c) {}

  A a;
  B b;
  C c;
};

#endif
