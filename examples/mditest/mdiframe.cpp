#include <precomp.h>
#include <wnd/wnds/os/ostoolbarwnd.h>
#include "mdiframe.h"
#include "mdichild.h"
#include "cmds.h"
#include "resource.h"
#include <commdlg.h>
#include <draw/bitmap.h>

MDIFrame *g_frame = NULL;

MDIFrame::MDIFrame() {
  g_frame = this;
  m_cmds = new Commands;
  setCommandCallback(m_cmds);
}

MDIFrame::~MDIFrame() {
  m_children.deleteAllSafe(); // we need the safe version whenever objects self unregister
  // don't delete toolbars, they are deleted by their host, because the host needs them in its destructor
  // no they don't, fuck, i have a leak, and i need to fix it :(
  g_frame = NULL;
  delete m_cmds;
}

int MDIFrame::onInit() {
  MDIFRAME_PARENT::onInit();

  setStatusBar(1);
  getStatusBarWnd()->addNewPart()->setText("OSStatusBarWnd");
  //allowMixedMode(0); // uncomment if you want all mdis to be maximized if one of them is

  PopupMenu *filemenu = new PopupMenu(MDITEST_FILE);
  filemenu->addCommand("&Open", MDITEST_FILE_OPEN);
  filemenu->addCommand("&Close", MDITEST_FILE_CLOSE);
  filemenu->addSeparator();
  filemenu->addCommand("&Exit", MDITEST_FILE_EXIT);
  addMenu("&File", 1, filemenu);

  PopupMenu *windowmenu = new PopupMenu(MDITEST_WINDOW);
  windowmenu->addCommand("&Cascade", MENU_COMMAND_CASCADE);
  windowmenu->addCommand("&Tile Vertically", MENU_COMMAND_VTILE);
  windowmenu->addCommand("Tile &Horizontally", MENU_COMMAND_HTILE); 
  windowmenu->addCommand("&Arrange Icons", MENU_COMMAND_ARRANGE);
  windowmenu->addSeparator();
  addMenu("&Window", 2, windowmenu);

  setIcon(IDI_MDITEST);

  m_toolbar = new OSToolBarWnd();
  m_toolbar->setCommandCallback(m_cmds);
  m_toolbar->addIconEntry(IDI_TOOLBAR_OPEN,16, "Open")->setCommand(MDITEST_FILE_OPEN);
  m_toolbar->addIconEntry(IDI_TOOLBAR_CLOSE,16, "Close")->setCommand(MDITEST_FILE_CLOSE);
  m_toolbar->addSeparator();
  m_toolbar->setParent(this);
  m_toolbar->setVisible(1);
  m_toolbar->setName("DockWnd 1");
  //m_toolbar->init(this);
  dockWindow(m_toolbar, DOCK_TOP); 

  // load the last docked saved state
  restoreDockedState();

  return 1;
}

int MDIFrame::onDeferredCallback(int param1, int param2) {
  switch (param1) {
    case DC_CLOSECHILD: {
      MDIChild *child = reinterpret_cast<MDIChild*>(param2);
      if (child) {
        delete child; // self unregisters
      }
      return 1;
    }
  }
  return MDIFRAME_PARENT::onDeferredCallback(param1, param2);
}

VOID MDIFrame::onOpen() {
  String tmpfile;
  OPENFILENAME l={sizeof(l),};
  l.lpstrFilter = "Image Files\0*.png;*.jpg\0";
  char *temp = (char *)GlobalAlloc(GMEM_FIXED,4096);
  temp[0] = 0;
  l.lpstrFile = temp;
  l.nMaxFile = 4095;
  l.lpstrTitle = "Load Image";
  l.lpstrDefExt = "";
  l.Flags = OFN_EXPLORER;
  if (GetOpenFileName(&l)) {
    tmpfile = temp;
  } else {
    GlobalFree(temp);
    return;
  }
  GlobalFree(temp);
  Bitmap *bmp = new Bitmap(tmpfile);
  if (bmp->isInvalid()) {
    MessageBox(gethWnd(), StringPrintf("Error loading %s", tmpfile.getValue()), "Error", 16);
    delete bmp;
  } else {
    MDIChild *child = new MDIChild(this, bmp);
    child->setName(tmpfile);
    child->setIcon(IDI_MDICHILD);
    addMDIChild(child); // self registers
  }
}
