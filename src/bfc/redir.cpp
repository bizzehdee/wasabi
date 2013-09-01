#include <precomp.h>
#include "redir.h"
#include <parse/paramparser.h>
#include <bfc/critsec.h>
#ifdef LINUX
#include <sys/fcntl.h>
#include <sys/poll.h>
#endif

#if !defined(WIN32) && !defined(LINUX)
#error port me!
#endif

int RedirPipeThread::threadProc() {
  char chBuf[PIPE_BUFSIZE];
  OSPIPE _out = getStdOut();
  #ifdef LINUX
  char chBufErr[PIPE_BUFSIZE];
  OSPIPE _err = getStdErr();
  #endif
  while (1) {
    if (getKillSwitch())
      break;
    *chBuf = 0;
    #ifdef WIN32
      DWORD dwRead=1;
      if (!ReadFile(getStdOut(), chBuf, PIPE_BUFSIZE, &dwRead, NULL) || dwRead == 0)
        break;
      chBuf[dwRead] = 0;
      onStdOutReceived(chBuf);
    #elif  defined(LINUX)
      *chBufErr = 0;

      int out_ready = 0;
      int err_ready = 0;

      pollfd pfderr = { _err, POLLIN|POLLPRI|POLLHUP, 0 };
      if (poll(&pfderr, 1, 2) < 0) {
        DebugString("poll error %d\n", errno);
      } else {
        if (pfderr.revents & (POLLIN|POLLPRI)) err_ready = 1;
        else if (pfderr.revents & POLLHUP) {
          setOffline();
          onTerminate();
          break;
        }
      }

      checkagain:

      if (getKillSwitch())
        break;

      pollfd pfd = { _out, POLLIN|POLLPRI|POLLHUP, 0 };
      if (poll(&pfd, 1, 2) < 0) {
        DebugString("poll error %d\n", errno);
      } else {
        if (pfd.revents & (POLLIN|POLLPRI)) out_ready = 1;
        else if (pfd.revents & POLLHUP) {
          setOffline();
          onTerminate();
          break;
        }
      }

      if (out_ready) {
        signed int dwRead;
        if ((dwRead = read(_out, chBuf, PIPE_BUFSIZE-1)) <= 0) {
          if (dwRead < 0) { _DebugString("out read error %d\n", errno); }
          setOffline();
          onTerminate();
        } else {
          chBuf[dwRead] = 0;
          onStdOutReceived(chBuf);
        }
        out_ready = 0;
        goto checkagain;
      }

      if (err_ready) {
        signed int dwRead;
        if ((dwRead = read(_err, chBufErr, PIPE_BUFSIZE-1)) <= 0) {
          if (dwRead < 0) { _DebugString("out read error %d\n", errno); }
        } else {
          chBufErr[dwRead] = 0;
          onStdOutReceived(chBufErr);
        }
      }

    #endif

    Sleep(5);
  }
  if (m_online) {
    setOffline();
    onTerminate();
  }
  return 0;
}

void RedirPipeThread::stdInSend(const char *data) {
  if (!m_online) return;
  DWORD dwWritten;
  #ifdef WIN32
    WriteFile(getStdIn(), data, STRLEN(data), &dwWritten, NULL);
  #elif defined(LINUX)
    write(getStdIn(), data, STRLEN(data));
  #endif
}

Redir::Redir(RedirPipeThread *rpt, const char *cmdline) {
  String cmd;
  m_processId = 0;
  if (!rpt) return;
  if (!cmdline || !*cmdline) {
#ifdef WIN32
    cmd = getenv("ComSpec");
    if (!cmd.isempty()) cmd.cat(" /A");
#elif defined(LINUX)
    cmd = "/bin/bash -i";
#endif
    cmdline = cmd.getValue();
    if (!cmdline || !*cmdline) return;
  }

  m_thread = rpt;

#ifdef WIN32
  SECURITY_ATTRIBUTES saAttr;
  int success;

  // Set the bInheritHandle flag so pipe handles are inherited.
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  // The steps for redirecting child process's STDOUT:
  //     1. Save current STDOUT, to be restored later.
  //     2. Create anonymous pipe to be STDOUT for child process.
  //     3. Set STDOUT of the parent process to be write handle to
  //        the pipe, so it is inherited by the child process.
  //     4. Create a noninheritable duplicate of the read handle and
  //        close the inheritable read handle.

  // Save the handle to the current STDOUT.
  m_hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);

  // Create a pipe for the child process's STDOUT.
  if( !CreatePipe( &m_hChildStdoutRd, &m_hChildStdoutWr, &saAttr, 0) ) {
    DebugString("Stdout pipe creation failed\n");
    return;
  }

  // Set a write handle to the pipe to be STDOUT.
  if (!SetStdHandle(STD_OUTPUT_HANDLE, m_hChildStdoutWr)) {
    DebugString("Redirecting STDOUT failed\n");
    return;
  }

  // Create noninheritable read handle and close the inheritable read handle.
  success = DuplicateHandle(GetCurrentProcess(), m_hChildStdoutRd,
        GetCurrentProcess(),  &m_hChildStdoutRdDup ,
        0,  FALSE,
        DUPLICATE_SAME_ACCESS );

  if (!success) {
    DebugString("DuplicateHandle failed\n");
    return;
  }
  CloseHandle( m_hChildStdoutRd );

  // The steps for redirecting child process's STDIN:
  //     1.  Save current STDIN, to be restored later.
  //     2.  Create anonymous pipe to be STDIN for child process.
  //     3.  Set STDIN of the parent to be the read handle to the
  //         pipe, so it is inherited by the child process.
  //     4.  Create a noninheritable duplicate of the write handle,
  //         and close the inheritable write handle.

  // Save the handle to the current STDIN.
  m_hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

  // Create a pipe for the child process's STDIN.
  if (!CreatePipe(&m_hChildStdinRd, &m_hChildStdinWr, &saAttr, 0)) {
    DebugString("Stdin pipe creation failed\n");
    return;
  }
  // Set a read handle to the pipe to be STDIN.
  if (!SetStdHandle(STD_INPUT_HANDLE, m_hChildStdinRd)) {
    DebugString("Redirecting Stdin failed\n");
    return;
  }
  // Duplicate the write handle to the pipe so it is not inherited.
  success = DuplicateHandle(GetCurrentProcess(), m_hChildStdinWr,
    GetCurrentProcess(), &m_hChildStdinWrDup,
    0, FALSE,                  // not inherited
    DUPLICATE_SAME_ACCESS );
  if (!success) {
    DebugString("DuplicateHandle failed\n");
    return;
  }
  CloseHandle(m_hChildStdinWr);

  // Now create the child process.
  if (!createProcess(cmdline)) {
    DebugString("CreateChildProcess failed\n");
    return;
  }
  // After process creation, restore the saved STDIN and STDOUT.
  if( !SetStdHandle(STD_INPUT_HANDLE, m_hSaveStdin)) {
    DebugString("Re-redirecting Stdin failed\n");
    return;
  }
  if (!SetStdHandle(STD_OUTPUT_HANDLE, m_hSaveStdout)) {
    DebugString("Re-redirecting Stdout failed\n");
    return;
  }
  m_thread->setStdOut(m_hChildStdoutRdDup);
  m_thread->setStdIn(m_hChildStdinWrDup);
#elif defined(LINUX)

  // create a pipe
  if (pipe(m_stdin_pipe) < 0 || pipe(m_stdout_pipe) < 0 || pipe(m_stderr_pipe) < 0) {
    DebugString("failed to make a pipe\n");
    return;
  }

  // create child
  m_processId = fork();

  // if pid==0 then its the child process
  if (!m_processId) {
    /* Close the Child process' STDIN */
    close(0);

    /* Duplicate the Child's STDIN to the stdin_pipe file descriptor */
    dup2(m_stdin_pipe[0], 0);

    /* Close the read and write to for the pipe for the child.  The child will now only be able to read from it's STDIN (which is our pipe). */
    close(m_stdin_pipe[0]);
    close(m_stdin_pipe[1]);

    /* Close the Child process' STDOUT */
    close(1);
    dup2(m_stdout_pipe[1], 1);
    close(m_stdout_pipe[0]);
    close(m_stdout_pipe[1]);

    /* Close the Child process' STDERR */
    close(2);
    dup2(m_stderr_pipe[1], 2);
    close(m_stderr_pipe[0]);
    close(m_stderr_pipe[1]);

    createProcess(cmdline);
    exit(0);
  }

  close(m_stdin_pipe[0]);
  close(m_stderr_pipe[1]);
  close(m_stdout_pipe[1]);

  setnonblock(m_stderr_pipe[0]);
  setnonblock(m_stdout_pipe[0]);

  m_thread->setStdErr(m_stderr_pipe[0]);
  m_thread->setStdOut(m_stdout_pipe[0]);
  m_thread->setStdIn(m_stdin_pipe[1]);

#endif

  m_thread->start();
}

void Redir::shutdown() {
  if (m_thread) {
    m_thread->setOffline();
    m_thread->setKillSwitch(1);
    #ifdef WIN32 // on win32, we block
    m_thread->kill();
    #else
    m_thread->end();
    #endif
    delete m_thread;
    m_thread = NULL;
    #ifdef WIN32
    CloseHandle(m_hChildStdinRd);
    CloseHandle(m_hChildStdoutWr);
    CloseHandle(m_hChildStdinWrDup);
    CloseHandle(m_hChildStdoutRdDup);
    if (m_processId != DWORD(-1))  {
      HANDLE hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, m_processId);
      if (hProcess) {
        TerminateProcess( hProcess,0 );
        CloseHandle( hProcess );
      }
    }
    #elif defined(LINUX)
    close(m_stdout_pipe[0]);
    close(m_stderr_pipe[0]);
    close(m_stdin_pipe[1]);
    m_stdout_pipe[0] = -1;
    m_stderr_pipe[0] = -1;
    m_stdin_pipe[1] = -1;
    #endif
  }
}

Redir::~Redir() {
  ASSERTPR(m_thread == NULL, "MUST call shutdown before destroying Redir");
}


DWORD Redir::createProcess(const char *cmdline) {
#ifdef WIN32
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;

  // Set up members of STARTUPINFO structure.
  ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
  siStartInfo.cb = sizeof(STARTUPINFO);

  siStartInfo.dwFlags = STARTF_USESTDHANDLES;
  siStartInfo.hStdInput = m_hChildStdinRd;
  siStartInfo.hStdOutput = m_hChildStdoutWr;
  siStartInfo.hStdError = m_hChildStdoutWr;

  _DebugString("Launching '%s'\n", cmdline);
  // Create the child process.
  BOOL ret = CreateProcess( NULL,
      (char *)cmdline,  // applicatin name
      NULL,             // process security attributes
      NULL,             // primary thread security attributes
      TRUE,             // handles are inherited
      DETACHED_PROCESS, // no console is allocate
      NULL,             // use parent's environment
      NULL,             // use parent's current directory
      &siStartInfo,     // STARTUPINFO pointer
      &piProcInfo);     // receives PROCESS_INFORMATION
  if (ret) {
    m_processId = piProcInfo.dwProcessId;
    return m_processId;
  }
  return 0;
#elif defined(LINUX)
  ParamParser pp(cmdline, " ");
  String s;
  for (int i=1;i<pp.getNumItems();i++) {
    if (!s.isempty()) s.cat(" ");
    s.cat(pp.enumItem(i));
  }
  _DebugString("Launching '%s' with params '%s'\n", pp.enumItem(0), s.getValue());
  execlp(pp.enumItem(0), pp.enumItem(0), s.getNonConstVal(), NULL);
#endif
}

#ifdef LINUX
void Redir::setnonblock(OSPIPE fd) {
  int fd_flags;
  if ((fd_flags = fcntl(fd, F_GETFL, 0)) == -1)
      _DebugString("Could not get flags for fd");
  else {
      fd_flags |= O_NONBLOCK;
      if (fcntl(fd, F_SETFL, fd_flags) == -1)
        _DebugString("Could not set flags for fd");
  }
}

void Redir::setblock(OSPIPE fd) {
  int fd_flags;
  if ((fd_flags = fcntl(fd, F_GETFL, 0)) == -1)
      _DebugString("Could not get flags for fd");
  else {
      fd_flags &= ~O_NONBLOCK;
      if (fcntl(fd, F_SETFL, fd_flags) == -1)
        _DebugString("Could not set flags for fd");
  }
}
#endif
