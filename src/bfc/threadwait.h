#ifndef THREADWAIT_H
#define THREADWAIT_H

class ThreadWait {
public:
  ThreadWait();
  virtual ~ThreadWait();

  void set();
  void clear();
  int wait(int timeoutms=-1);

  void init(const char *name);
  void shutdown();

private:
  HANDLE m_event;
};


#endif
