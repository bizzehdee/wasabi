#include <precomp.h>
#include "bigcounter.h"

BigCounterRoot::BigCounterRoot() {
  m_nelem = 0;
}

BigCounterRoot::BigCounterRoot(BigCounterRoot *src) {
  m_nelem = src->bigcounter_getNumWords();
  bigcounter_createIntArray(m_nelem);
  set(src);
}

BigCounterRoot::~BigCounterRoot() {
  bigcounter_destroyIntArray();
}

void BigCounterRoot::init(int size_in_bits) {
  float nelem = ((float)size_in_bits/(float)sizeof(int)/8);
  if ((float)((int)nelem) != nelem) nelem++;
  m_nelem = (int)nelem;
  bigcounter_createIntArray(m_nelem);
}

void BigCounterRoot::increment() {
  INCRITICALSECTION(m_cs);
  unsigned int *table = (unsigned int *)bigcounter_lock(1);
  if (!_inc(table)) { // full rollover
    MEMSET(table, 0, m_nelem*sizeof(int));
    bigcounter_onRollover();
  }
  bigcounter_onUpdate(table);
  bigcounter_unlock(1);
}

void BigCounterRoot::decrement() {
  INCRITICALSECTION(m_cs);
  unsigned int *table = (unsigned int *)bigcounter_lock(1);
  if (!_dec(table)) { // full rollover
    MEMSET(table, 0xFF, m_nelem*sizeof(int));
    bigcounter_onRollover();
  }
  bigcounter_onUpdate(table);
  bigcounter_unlock(1);
}

int BigCounterRoot::compare(BigCounterRoot *b) {
  int r;
  INCRITICALSECTION(m_cs);
  {
   INCRITICALSECTION(b->m_cs);
   ASSERT(b->bigcounter_getNumWords() == m_nelem);
   unsigned int *table_a = (unsigned int *)bigcounter_lock();
   unsigned int *table_b = (unsigned int *)b->bigcounter_lock();
   r = _cmp(&table_a[m_nelem-1], &table_b[m_nelem-1], m_nelem-1);
   bigcounter_unlock();
   b->bigcounter_unlock();
  }
  return r;
}

void BigCounterRoot::set(BigCounterRoot *b) {
  if (b == this) return;
  INCRITICALSECTION(m_cs);
  ASSERT(b->bigcounter_getNumWords() == m_nelem);
  unsigned int *table_a = (unsigned int *)bigcounter_lock(1);
  unsigned int *table_b = (unsigned int *)b->bigcounter_lock();
  MEMCPY(table_a, table_b, m_nelem*sizeof(int));
  bigcounter_onUpdate(table_a);
  bigcounter_unlock(1);
  b->bigcounter_unlock();
}

void BigCounterRoot::reset() {
  INCRITICALSECTION(m_cs);
  unsigned int *table_a = (unsigned int *)bigcounter_lock(1);
  MEMSET(table_a, 0, bigcounter_getNumWords()*sizeof(int));
  bigcounter_onUpdate(table_a);
  bigcounter_unlock(1);
}

void BigCounterRoot::setFromTable(int *table_b, int n) {
  if ((BigCounterRoot *)table_b == this) return;
  ASSERT(n == m_nelem);
  unsigned int *table_a = (unsigned int *)bigcounter_lock(1);
  MEMCPY(table_a, table_b, m_nelem*sizeof(int));
  bigcounter_onUpdate(table_a);
  bigcounter_unlock(1);
}

void BigCounterRoot::copyToTable(int *table_b, int n) {
  if ((BigCounterRoot *)table_b == this) return;
  ASSERT(n == m_nelem);
  unsigned int *table_a = (unsigned int *)bigcounter_lock();
  MEMCPY(table_b, table_a, m_nelem*sizeof(int));
  bigcounter_unlock();
}

int BigCounterRoot::_inc(unsigned int *ptr, int n) {
  if (n == m_nelem) return 0;
  if (*ptr == (unsigned int)-1) { // rollover
    *ptr = 0;
    if (!_inc(ptr+1, n+1)) return 0;
  } else {
    *ptr++;
  }
  return 1;
}

int BigCounterRoot::_dec(unsigned int *ptr, int n) {
  if (n == m_nelem) return 0;
  if (*ptr == 0) { // rollover
    *ptr = (unsigned int)-1;
    if (!_inc(ptr+1, n+1)) return 0;
  } else {
    *ptr--;
  }
  return 1;
}

int BigCounterRoot::_cmp(unsigned int *ptr_a, unsigned int *ptr_b, int n) {
  if (n < 0) return 0;
  if (*ptr_a < *ptr_b) return -1;
  if (*ptr_a > *ptr_b) return 1;
  return _cmp(ptr_a-1, ptr_b-1, n-1);
}

