#ifndef MDITEST_H
#define MDITEST_H

#include <wnd/virtualwnd.h>

class MDIFrame;

class MDITest {
public:
  MDITest();
  virtual ~MDITest();

  void onInit();
  void onShutdown();

public:
	int run(HINSTANCE instance, const char *cmdline);

private:
  MDIFrame *m_frame;
};

#endif
