#ifndef _GENWND_H
#define _GENWND_H

#include <api/wnd/basewnd.h>

#define GENWND_PARENT BaseWnd

typedef int (*userTimerProc)(int id, void *data1, void *data2);

typedef struct {
  userTimerProc fn;
  int id;
  void *data1;
  void *data2;
} userTimerProcStruct;

typedef void (*deferredCallbackFunction)(int p1, int p2);
#define ID_FUNC 0x2842

typedef struct {
  deferredCallbackFunction f;
  int p1;
  int p2;
} cbft;

class GenWnd : public GENWND_PARENT {
  public:
    virtual ~GenWnd();
    
    void postDeferredFunctionCallback(int p1, int p2, deferredCallbackFunction f);
    int onDeferredCallback(int p1, int p2);
    void timerCallback(int id);
    virtual int onPostOnInit();

    static int allocateNewTimer(int delay, userTimerProc p, void *d1, void *d2);
    static void deallocateTimer(int id);

    static void setTimerGenWnd(GenWnd *t);

    static PtrList<userTimerProcStruct> timerprocs;
    static int hi;
    static GenWnd *genwnd;
};


#endif
