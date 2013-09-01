#include <precomp.h>
#include <api/apiinit.h>
#include <api/service/svcmgr.h>
#include <bfc/file/readdir.h>
#include <bfc/ptrlist.h>
#include <wac/wacontainer.h>
#include <bfc/std.h>
#include <bfc/string/string.h>

DECLARE_MODULE_SVCMGR

PtrList<waContainer> m_wacs;
String m_wacdir;
int n=0;

// {EE58DBE5-4F33-47c4-8438-CEC85DE4AAAB}
static const GUID myappguid =
{ 0xee58dbe5, 0x4f33, 0x47c4, { 0x84, 0x38, 0xce, 0xc8, 0x5d, 0xe4, 0xaa, 0xab } };

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	ApiInit::init(hInstance, myappguid, lpCmdLine, (HWND)NULL);

	//get our wac dir
	char m_dir[MAX_PATH];
	Std::getCurDir(m_dir, MAX_PATH);
	m_wacdir = m_dir;
	m_wacdir.cat("wacs/");

	//load our wacs
	ReadDir m_read(m_wacdir, "*.wac");
	while(m_read.next()) {
		String m_full = m_read.getPath();
		m_full += m_read.getFilename();
		m_wacs.addItem(new waContainer(m_full));
	}
	//all wacs are loaded
	//now we have all api's

	//call oncreate on all wac-c's
  foreach(m_wacs)
    m_wacs.getfor()->callCreate();
  endfor

	//do what you wanna do with your app....

	// ....

	//time to shut down

	//destroy all wac-c's
  foreach(m_wacs)
    m_wacs.getfor()->callDestroy();
  endfor
	// all wac-c's gone, we dont need api's anymore :)

	//kill all wac's
	//they are unloaded and released back into the wild automaticaly :)
	m_wacs.deleteAll();

	ApiInit::shutdown();
	return 0;
}
