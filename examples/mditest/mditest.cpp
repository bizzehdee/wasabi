#include <precomp.h>
#include "mditest.h"
#include <api/apiinit.h>
#include <commctrl.h>
#include <shellapi.h>
#include <api/application/version.h>
#include "mdiframe.h"

DECLARE_MODULE_SVCMGR

#pragma comment(linker, "/include:__link_PngLoadSvc")
#pragma comment(linker, "/include:__link_JpgLoadSvc")

// {6A6A3D51-1D0E-4c5e-9332-9E1513A9E96A}
static const GUID mditestGuid = 
{ 0x4cf3f39b, 0x8a28, 0x4cbe, { 0x8d, 0xc3, 0x1e, 0x34, 0xf4, 0x1c, 0x5d, 0x92 } };

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
  MDITest mditest;
  return mditest.run(hInstance, lpCmdLine);
}

int MDITest::run(HINSTANCE instance, const char *cmdline) {
  InitCommonControls();
  if (!ApiInit::init(instance, mditestGuid, cmdline, (HWND)NULL)) return 0;
  WasabiVersion::setAppName("WasabiMDITest");
  onInit();
  while (!WASABI_API_APP->main_isShuttingDown()) { 
    if (!WASABI_API_APP->app_messageLoopStep()) break; 
    Sleep(1);
  }
  onShutdown();
  ApiInit::shutdown();
  return 1;
}

MDITest::MDITest() {
  m_frame = NULL;
}

MDITest::~MDITest() {
}

void MDITest::onInit() {
  m_frame = new MDIFrame();
  m_frame->resize(300, 200, 800, 600);
  m_frame->setName("OSMDIParentWnd");
  m_frame->setStartHidden(1);
  m_frame->init(WASABI_API_APP->main_gethInstance(), NULL, TRUE);
  m_frame->setVisible(1);
  m_frame->cascadeMDI();
}

void MDITest::onShutdown() {
  delete m_frame; m_frame = NULL;
}

