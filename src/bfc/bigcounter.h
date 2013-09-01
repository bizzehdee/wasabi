#ifndef BIGCOUNTER_H
#define BIGCOUNTER_H

#include <bfc/memblock.h>
#include <bfc/sharedmem.h>
#include <bfc/critsec.h>

class BigCounterRoot {
protected:
  BigCounterRoot();
  BigCounterRoot(BigCounterRoot *src);
  virtual ~BigCounterRoot();

public:

  virtual void init(int size_in_bits);

  void increment();
  void decrement();
  int compare(BigCounterRoot *b);
  void set(BigCounterRoot *b);
  void reset();
  void setFromTable(int *table_b, int n);
  void copyToTable(int *table_b, int n);

  int bigcounter_getNumWords() { return m_nelem; }

  virtual void bigcounter_onRollover() {}
  virtual void bigcounter_onUpdate(unsigned int *table) {}

  virtual void bigcounter_createIntArray(int nelements) {}
  virtual void bigcounter_destroyIntArray() {}
  virtual int *bigcounter_lock(int write=0)=0;
  virtual void bigcounter_unlock(int write=0)=0;

protected:
  int _inc(unsigned int *ptr, int n=0);
  int _dec(unsigned int *ptr, int n=0);
  int _cmp(unsigned int *ptr_a, unsigned int *ptr_b, int n);

private:
  int m_nelem;
  CriticalSection m_cs;
};

class BigCounter : public BigCounterRoot {
public:
  BigCounter() {}
  virtual ~BigCounter() {}

  virtual void init(int size_in_bits) {
    BigCounterRoot::init(size_in_bits);
    reset();
  }

  virtual void bigcounter_createIntArray(int nelements) {
    m_words.setSize(nelements);
  }
  virtual void bigcounter_destroyIntArray() {
    m_words.setSize(0);
  }
  virtual int *bigcounter_lock(int write=0) {
    m_cs.enter();
    return m_words.getMemory();
  }
  virtual void bigcounter_unlock(int write=0) {
    m_cs.leave();
  }

protected:
  MemBlock<int> m_words;
  CriticalSection m_cs;
};

class BigCounterShared : public BigCounter {
public:
  BigCounterShared() {}
  virtual ~BigCounterShared() {}

  virtual void init(const char *ipcname, int size_in_bits) {
    m_name = ipcname;
    BigCounter::init(size_in_bits);
  }

  virtual void bigcounter_createIntArray(int nelements) {
    BigCounter::bigcounter_createIntArray(nelements);
    m_shwords.init(nelements*sizeof(int)/sizeof(char), m_name);
  }
  virtual void bigcounter_destroyIntArray() {
    BigCounter::bigcounter_destroyIntArray();
    m_shwords.shutdown();
  }
  virtual int *bigcounter_lock(int write=0) {
    if (!write) return BigCounter::bigcounter_lock();
    return (int *)m_shwords.lock();
  }
  virtual void bigcounter_unlock(int write=0) {
    if (!write) { BigCounter::bigcounter_unlock(); return; }
    m_shwords.unlock();
  }

  void bigcounter_onUpdate(int *table) {
    m_words.setMemory(table, sizeof(int)*bigcounter_getNumWords());
  }

private:
  SharedMem m_shwords;
  String m_name;
};

#endif
