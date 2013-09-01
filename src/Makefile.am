SUBDIRS = wconfig

AM_CPPFLAGS = @wa_cppflags@
AM_CFLAGS = @cf_cflags@
AM_CXXFLAGS = @cf_cflags@

if DEBUG
AM_CFLAGS += -g
AM_CXXFLAGS += -g
AM_CPPFLAGS += -DEBUG
endif

bin_PROGRAMS = wasabitest testapp

wasabitest_SOURCES =  wasabitest.cpp
wasabitest_LDADD = $(libwasabi_ldadd)
wasabitest_CPPFLAGS = $(AM_CPPFLAGS)

testapp_SOURCES = main.cpp app.cpp
testapp_LDADD = $(libwasabi_ldadd)
testapp_CPPFLAGS = $(AM_CPPFLAGS)

noinst_LIBRARIES =  libwasabi.a

# #################################
# libwasabi.a
# #################################

libwasabi_a_SOURCES = precomp.cpp
libwasabi_a_CPPFLAGS = $(AM_CPPFLAGS)
libwasabi_a_CFLAGS = $(AM_CFLAGS)
libwasabi_a_CXXFLAGS =  $(AM_CXXFLAGS)

# anything build with libwasabi.a will need these libs
# (note, not an Automake variable, so add it to each prog_LDADD)
libwasabi_ldadd = libwasabi.a -luuid @x_libs@

if WA_WND
libwasabi_a_CFLAGS += @x_cflags@
libwasabi_a_CXXFLAGS += @x_cflags@
endif

##
## bfc
##

## bfc/std_*

libwasabi_a_SOURCES += \
bfc/std.cpp bfc/std.h \
bfc/std_cpu.cpp bfc/std_cpu.h \
bfc/std_file.cpp bfc/std_file.h \
bfc/std_math.cpp bfc/std_math.h \
bfc/std_mem.cpp bfc/std_mem.h \
bfc/std_path.cpp bfc/std_path.h \
bfc/std_rand.cpp bfc/std_rand.h \
bfc/std_string.cpp bfc/std_string.h

if WA_WND
libwasabi_a_SOURCES += bfc/std_wnd.cpp bfc/std_wnd.h
endif

## bfc/platform
libwasabi_a_SOURCES += bfc/platform/posix/posix.cpp bfc/platform/platform.cpp
if WA_WND
libwasabi_a_SOURCES += bfc/platform/xwindow/xwindow.cpp
endif

## bfc/atrib
libwasabi_a_SOURCES += \
bfc/attrib/attrbool.h bfc/attrib/attrcb.h \
bfc/attrib/attrcolor.cpp bfc/attrib/attrcolor.h \
bfc/attrib/attrfloat.h \
bfc/attrib/attrfn.h \
bfc/attrib/attrhandler.h \
bfc/attrib/attribs.h \
bfc/attrib/attribute.cpp bfc/attrib/attribute.h \
bfc/attrib/attrint.h \
bfc/attrib/attrstr.cpp bfc/attrib/attrstr.h \
bfc/attrib/cfgitem.h \
bfc/attrib/cfgitemi.cpp bfc/attrib/cfgitemi.h

## bfc/file
libwasabi_a_SOURCES += \
bfc/file/choosedir.cpp bfc/file/choosedir.h \
bfc/file/filename.cpp bfc/file/filename.h \
bfc/file/readdir.cpp bfc/file/readdir.h \
bfc/file/recursedir.cpp bfc/file/recursedir.h \
bfc/file/splitpath.c bfc/file/splitpath.h \
bfc/file/tmpnamestr.h \
bfc/file/wildcharsenum.cpp bfc/file/wildcharsenum.h
## don't compile:
## bfc/file/filemon.cpp bfc/file/filemon.h
## bfc/file/openfile.cpp bfc/file/openfile.h
##bfc/file/savefile.cpp bfc/file/savefile.h

## bfc/util
libwasabi_a_SOURCES += \
bfc/util/bindecode.cpp bfc/util/bindecode.h \
bfc/util/binencode.cpp bfc/util/binencode.h \
bfc/util/findopenrect.cpp bfc/util/findopenrect.h \
bfc/util/inifile.cpp bfc/util/inifile.h \
bfc/util/profile.c bfc/util/profile.h \
bfc/util/profiler.cpp bfc/util/profiler.h \
bfc/util/timefmt.cpp bfc/util/timefmt.h

## bfc/string
libwasabi_a_SOURCES += \
bfc/string/bigstring.cpp bfc/string/bigstring.h \
bfc/string/playstring.cpp bfc/string/playstring.h \
bfc/string/string.cpp bfc/string/string.h \
bfc/string/stringMap.cpp bfc/string/stringMap.h \
bfc/string/stringPair.cpp bfc/string/stringPair.h \
bfc/string/stringdict.h \
bfc/string/stringpool.cpp bfc/string/stringpool.h \
bfc/string/stringtable.cpp bfc/string/stringtable.h \
bfc/string/url.cpp bfc/string/url.h \
bfc/string/utf8.cpp bfc/string/utf8.h

if WA_SVC
libwasabi_a_SOURCES += \
bfc/string/encodedstr.cpp bfc/string/encodedstr.h \
bfc/string/redirstr.cpp bfc/string/redirstr.h
endif

## bfc
libwasabi_a_SOURCES += \
bfc/Buffer.cpp bfc/Buffer.h \
bfc/assert.cpp bfc/assert.h \
bfc/bigcounter.cpp bfc/bigcounter.h \
bfc/bitlist.h \
bfc/circbuf.h \
bfc/common.h \
bfc/compare.h \
bfc/critsec.cpp bfc/critsec.h \
bfc/depend.cpp bfc/depend.h \
bfc/depview.h \
bfc/dispatch.h \
bfc/foreach.cpp bfc/foreach.h \
bfc/freelist.cpp bfc/freelist.h \
bfc/guid.cpp bfc/guid.h \
bfc/loadlib.cpp bfc/loadlib.h \
bfc/map.h \
bfc/memblock.cpp bfc/memblock.h \
bfc/multimap.h \
bfc/mutex.cpp bfc/mutex.h \
bfc/named.h \
bfc/pair.h \
bfc/paramobj.cpp bfc/paramobj.h \
bfc/ptrlist.cpp bfc/ptrlist.h \
bfc/rect.cpp bfc/rect.h \
bfc/redir.cpp bfc/redir.h \
bfc/reentryfilter.h \
bfc/sharedmem.cpp bfc/sharedmem.h \
bfc/sourcesink.cpp bfc/sourcesink.h \
bfc/stack.cpp bfc/stack.h \
bfc/thread.cpp bfc/thread.h \
bfc/tlist.h
## -- deprecated
## bfc/nsguid.cpp bfc/nsguid.h \
## -- don't compile:
## bfc/threadwait.cpp bfc/threadwait.h \


##
## wnd & draw
##

if WA_WND

libwasabi_a_SOURCES += \
wnd/accessible.cpp wnd/accessible.h \
wnd/backbufferwnd.cpp wnd/backbufferwnd.h \
wnd/basewnd.cpp wnd/basewnd.h \
wnd/bltcanvas.cpp wnd/bltcanvas.h \
wnd/canvas.cpp wnd/canvas.h \
wnd/clickwnd.cpp wnd/clickwnd.h \
wnd/cursor.cpp wnd/cursor.h \
wnd/dockhostwnd.cpp wnd/dockhostwnd.h \
wnd/drag.h \
wnd/findobjectcb.cpp wnd/findobjectcb.h \
wnd/labelwnd.cpp wnd/labelwnd.h \
wnd/metricscb.cpp wnd/metricscb.h \
wnd/notifmsg.h \
wnd/paintcb.cpp wnd/paintcb.h \
wnd/physwnd.h \
wnd/region.cpp wnd/region.h \
wnd/rootwnd.cpp wnd/rootwnd.h \
wnd/scbkgwnd.cpp wnd/scbkgwnd.h \
wnd/scrollbar.cpp wnd/scrollbar.h \
wnd/usermsg.h \
wnd/virtualwnd.cpp wnd/virtualwnd.h \
wnd/wndevent.h
## -- don't compile
## wnd/popup.cpp wnd/popup.h \
## wnd/subclass.cpp wnd/subclass.h \
## wnd/systray.cpp wnd/systray.h \
## wnd/tooltip.cpp wnd/tooltip.h \


## might not compile:
## wnd/dockhostwnd.cpp wnd/dockhostwnd.h wnd/popup.cpp wnd/popup.h wnd/subclass.cpp wnd/subclass.h wnd/systray.cpp wnd/systray.h wnd/tooltip.cpp wnd/tooltip.h 

## wnd/wnds
libwasabi_a_SOURCES += \
wnd/wnds/blankwnd.cpp
##wnd/wnds/editwnd.cpp
##wnd/wnds/framewnd.cpp
##wnd/wnds/listwnd.cpp
##wnd/wnds/browser/Atl.cpp
##wnd/wnds/browser/minibrowser.cpp
##wnd/wnds/browser/browserwnd.cpp
##wnd/wnds/dockwnd.cpp
##wnd/wnds/treewnd.cpp

## draw
libwasabi_a_SOURCES += \
draw/ahsv.h \
draw/argb.h \
draw/argb32.h \
draw/bitmap.cpp draw/bitmap.h \
draw/blending.cpp draw/blending.h \
draw/blendrect.cpp draw/blendrect.h \
draw/contrast.cpp draw/contrast.h \
draw/convolve.cpp draw/convolve.h \
draw/drawpoly.cpp draw/drawpoly.h \
draw/f_saturate.h \
draw/filter.h \
draw/filteredcolor.cpp draw/filteredcolor.h \
draw/floodfill.cpp draw/floodfill.h \
draw/gradient.cpp draw/gradient.h \
draw/histo.cpp draw/histo.h
## -- don't compile
## draw/skinfilter.cpp draw/skinfilter.h

endif

##
## parse
##

libwasabi_a_SOURCES += \
parse/hierarchyparser.cpp parse/hierarchyparser.h \
parse/node.cpp parse/node.h \
parse/paramparser.cpp parse/paramparser.h \
parse/pathparse.cpp parse/pathparse.h \
parse/uriparse.cpp parse/uriparse.h \
parse/urlparse.cpp parse/urlparse.h

##
## APIs
##

if WA_SVC

libwasabi_a_SOURCES += api/apiinit.cpp api/apiinit.h

libwasabi_a_SOURCES += \
api/service/api_service.cpp api/service/api_service.h \
api/service/api_servicei.cpp api/service/api_servicei.h \
api/service/api_servicex.cpp api/service/api_servicex.h \
api/service/service.h \
api/service/servicei.cpp api/service/servicei.h \
api/service/services.h \
api/service/svc_enum.cpp api/service/svc_enum.h \
api/service/svccache.cpp api/service/svccache.h \
api/service/svcenum.cpp api/service/svcenum.h \
api/service/svcenumbyguid.cpp api/service/svcenumbyguid.h \
api/service/svcenumt.cpp api/service/svcenumt.h \
api/service/svcmgr.cpp api/service/svcmgr.h \
api/service/waservicefactory.cpp api/service/waservicefactory.h \
api/service/waservicefactorybase.cpp api/service/waservicefactorybase.h \
api/service/waservicefactoryi.cpp api/service/waservicefactoryi.h \
api/service/waservicefactoryt.cpp api/service/waservicefactoryt.h \
api/service/waservicefactorytsingle.cpp api/service/waservicefactorytsingle.h \
api/service/waservicefactoryx.cpp api/service/waservicefactoryx.h

endif

if WA_APP

libwasabi_a_SOURCES += \
api/application/api_application.cpp api/application/api_application.h \
api/application/api_applicationi.cpp api/application/api_applicationi.h \
api/application/api_applicationx.cpp api/application/api_applicationx.h \
api/application/ipcs.cpp api/application/ipcs.h \
api/application/pathmgr.cpp api/application/pathmgr.h \
api/application/version.cpp api/application/version.h \
api/application/wkc.cpp api/application/wkc.h

endif

if WA_SYSCB

libwasabi_a_SOURCES += \
api/syscb/api_syscb.cpp api/syscb/api_syscb.h \
api/syscb/api_syscbi.cpp api/syscb/api_syscbi.h \
api/syscb/api_syscbx.cpp api/syscb/api_syscbx.h \
api/syscb/cbmgr.cpp api/syscb/cbmgr.h
## api/syscb/callbacks
libwasabi_a_SOURCES += \
api/syscb/callbacks/consolecb.cpp api/syscb/callbacks/consolecb.h \
api/syscb/callbacks/corecb.cpp api/syscb/callbacks/corecb.h \
api/syscb/callbacks/gccb.h \
api/syscb/callbacks/runlevelcb.h \
api/syscb/callbacks/svccb.h \
api/syscb/callbacks/syscb.cpp api/syscb/callbacks/syscb.h \
api/syscb/callbacks/syscbi.cpp api/syscb/callbacks/syscbi.h \
api/syscb/callbacks/syscbx.cpp api/syscb/callbacks/syscbx.h \
api/syscb/callbacks/sysmemcb.h \
api/syscb/callbacks/wndcb.cpp api/syscb/callbacks/wndcb.h
## -- don't compile
## api/syscb/callbacks/skincb.cpp api/syscb/callbacks/skincb.h \

endif

if WA_LINUX

libwasabi_a_SOURCES += \
api/linux/api_linux.cpp api/linux/api_linux.h \
api/linux/api_linuxi.cpp api/linux/api_linuxi.h \
api/linux/api_linuxx.cpp api/linux/api_linuxx.h \
api/linux/linuxapi.cpp api/linux/linuxapi.h

endif

if WA_TIMERS

libwasabi_a_SOURCES += \
api/timer/api_timer.cpp api/timer/api_timer.h \
api/timer/timerapi.cpp api/timer/timerapi.h \
api/timer/timerclient.cpp api/timer/timerclient.h \
api/timer/timermul.cpp api/timer/timermul.h \
api/timer/timeslicer.cpp api/timer/timeslicer.h \
api/timer/tmultiplex.cpp api/timer/tmultiplex.h
## -- don't compile
## api/timer/genwnd.cpp api/timer/genwnd.h \

endif

if WA_TEXTMODE

libwasabi_ldadd += -lncurses

libwasabi_a_SOURCES += \
api/textmode/api_textmode.cpp api/textmode/api_textmode.h \
api/textmode/api_textmodei.cpp api/textmode/api_textmodei.h \
api/textmode/api_textmodex.cpp api/textmode/api_textmodex.h \
api/textmode/ncurseskey.cpp api/textmode/ncurseskey.h \
api/textmode/textmode_ncurses.cpp api/textmode/textmode_ncurses.h \
api/textmode/tm_kbdfocus.cpp api/textmode/tm_kbdfocus.h \
api/textmode/tm_kbdfocusi.cpp api/textmode/tm_kbdfocusi.h \
api/textmode/tm_kbdfocusx.cpp api/textmode/tm_kbdfocusx.h \
api/textmode/tm_menu.cpp api/textmode/tm_menu.h
## --don't compile
## api/textmode/textmode_win32.cpp api/textmode/textmode_win32.h \

## api/textmode/TUI
libwasabi_a_SOURCES += \
api/textmode/TUI/TUI.cpp api/textmode/TUI/TUI.h \
api/textmode/TUI/TUICanvas.cpp api/textmode/TUI/TUICanvas.h \
api/textmode/TUI/TUIDefs.h \
api/textmode/TUI/TUIObject.cpp api/textmode/TUI/TUIObject.h \
api/textmode/TUI/TUIObject_EventSink.cpp api/textmode/TUI/TUIObject_EventSink.h \
api/textmode/TUI/TUIObject_EventSource.cpp api/textmode/TUI/TUIObject_EventSource.h \
api/textmode/TUI/TUISprite.cpp api/textmode/TUI/TUISprite.h \
api/textmode/TUI/TUISprite_EventSink.cpp api/textmode/TUI/TUISprite_EventSink.h \
api/textmode/TUI/TUISprite_EventSource.cpp api/textmode/TUI/TUISprite_EventSource.h \
api/textmode/TUI/TUITerm.cpp api/textmode/TUI/TUITerm.h \
api/textmode/TUI/TUITerm_EventSink.cpp api/textmode/TUI/TUITerm_EventSink.h \
api/textmode/TUI/TUITerm_EventSource.cpp api/textmode/TUI/TUITerm_EventSource.h \
api/textmode/TUI/TUIText.cpp api/textmode/TUI/TUIText.h \
api/textmode/TUI/TUIText_EventSink.cpp api/textmode/TUI/TUIText_EventSink.h \
api/textmode/TUI/TUIText_EventSource.cpp api/textmode/TUI/TUIText_EventSource.h


endif

if WA_XMLPARSER

libwasabi_a_SOURCES += \
api/xml/api_xml.cpp api/xml/api_xml.h \
api/xml/xmlapi.cpp api/xml/xmlapi.h \
api/xml/xmlconfig.cpp api/xml/xmlconfig.h \
api/xml/xmlmanagedobj.cpp api/xml/xmlmanagedobj.h \
api/xml/xmlmanager.h \
api/xml/xmlobject.cpp api/xml/xmlobject.h \
api/xml/xmlparams.cpp api/xml/xmlparams.h \
api/xml/xmlparamsi.cpp api/xml/xmlparamsi.h \
api/xml/xmlparamsx.cpp api/xml/xmlparamsx.h \
api/xml/xmlparse.h \
api/xml/xmlreader.cpp api/xml/xmlreader.h \
api/xml/xmlstrparams.h \
api/xml/xmlwrite.cpp api/xml/xmlwrite.h
## api/xml/parser
libwasabi_a_SOURCES += \
api/xml/parser/asciitab.h \
api/xml/parser/hashtable.c api/xml/parser/hashtable.h \
api/xml/parser/iasciitab.h \
api/xml/parser/latin1tab.h \
api/xml/parser/nametab.h \
api/xml/parser/simap.cpp api/xml/parser/simap.h \
api/xml/parser/utf8tab.h \
api/xml/parser/xmldef.h \
api/xml/parser/xmlparse.h \
api/xml/parser/xmlparser.c \
api/xml/parser/xmlrole.c api/xml/parser/xmlrole.h \
api/xml/parser/xmltok.c api/xml/parser/xmltok.h
## -- not needed?
## api/xml/parser/xmltok_impl.c api/xml/parser/xmltok_impl.h \
## api/xml/parser/xmltok_ns.c

endif
