#ifndef _REDIR_H
#define _REDIR_H

#include <bfc/thread.h>
#include <bfc/string/string.h>

#define PIPE_BUFSIZE 4096

class RedirPipeThread : public Thread {
public:
  RedirPipeThread() : m_stdin(0), m_stdout(0), m_stderr(0), m_online(1) {}
  virtual ~RedirPipeThread() {}

  // the redirected process has written text in its stdout, override to get the data
  virtual void onStdOutReceived(const char *data) { }
  // call to send data to the redirected process' stdin
  virtual void stdInSend(const char *data);

  // the redirected process has terminated on its own, the thread is about to return
  virtual void onTerminate() { }
  void setOffline() { m_online = 0; }


  // call these to get the redirected process' std in/out
  virtual OSPIPE getStdIn() { return m_stdin; }
  virtual OSPIPE getStdOut() { return m_stdout; }
  virtual OSPIPE getStdErr() { return m_stderr; }

  // these are called by Redir
  void setStdIn(OSPIPE in) { m_stdin = in; }
  void setStdOut(OSPIPE out) { m_stdout = out; }
  void setStdErr(OSPIPE err) { m_stderr = err; }

  // the default implementation of threadProc already calls onStdOutReceived() and onTerminate(); override it to add extra work
  virtual int threadProc();

private:
  OSPIPE m_stdin;
  OSPIPE m_stdout;
  OSPIPE m_stderr;
  int m_online;
};

class Redir {
public:
  Redir(RedirPipeThread *rpt, const char *cmdline=NULL);
  virtual ~Redir();

  // you must call this before deleting the object.
  void shutdown();

#ifdef LINUX
  void setnonblock(OSPIPE pipe);
  void setblock(OSPIPE pipe);
#endif

private:
  DWORD createProcess(const char *cmdline);
  RedirPipeThread *m_thread;

#ifdef WIN32
  OSPIPE m_hChildStdinRd, m_hChildStdinWr, m_hChildStdinWrDup,
    m_hChildStdoutRd, m_hChildStdoutWr, m_hChildStdoutRdDup,
    m_hSaveStdin, m_hSaveStdout;
#elif defined(LINUX)
  OSPIPE m_defin;
  OSPIPE m_defout;
  OSPIPE m_stdin_pipe[2];
  OSPIPE m_stdout_pipe[2];
  OSPIPE m_stderr_pipe[2];
#endif
  OSPROCESSID m_processId;
};

#endif


