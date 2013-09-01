#ifndef _SVCENUM_H
#define _SVCENUM_H


/*<?<autoheader/>*/
class waServiceFactory;
/*?>*/

// abstract base class
class NOVTABLE COMEXP SvcEnum {
protected:
  SvcEnum();

  void *_getNext(int global_lock = TRUE);
  void reset();

  virtual int _testService(void *)=0;

public:
#ifdef ASSERTS_ENABLED
  static int release(waServiceFactory *ptr) { ASSERTALWAYS("never ever call release() with a waServiceFactory * !!!"); return 0; }
#endif

  //NOTE: release() won't work if getNext() was called with global_lock=FALSE
  // instead, you must release through the factory directly (getLastFactory()->releaseInterface(ptr)).
  static int release(void *ptr);

  waServiceFactory *getLastFactory();

protected:
  GUID type;

private:
  int pos;
  waServiceFactory * factory;
};


#endif // _SVCENUM_H

