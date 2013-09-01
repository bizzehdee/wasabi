# Microsoft Developer Studio Project File - Name="wasabi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=wasabi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "wasabi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "wasabi.mak" CFG="wasabi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "wasabi - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "wasabi - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "wasabi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "_build/Release"
# PROP Intermediate_Dir "_build/Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zd /O2 /I "./" /I "../../wasabi/src" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"precomp.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "wasabi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "_build/Debug"
# PROP Intermediate_Dir "_build/Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./" /I "../../wasabi/src" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"precomp.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "wasabi - Win32 Release"
# Name "wasabi - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "system"

# PROP Default_Filter ""
# Begin Group "file"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\file\filename.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\file\readdir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\file\recursedir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\file\wildcharsenum.cpp
# End Source File
# End Group
# Begin Group "memory"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\foreach.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\freelist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\loadlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\memblock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\ptrlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\stack.cpp
# End Source File
# End Group
# Begin Group "utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\util\findopenrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\util\profiler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\util\timefmt.cpp
# End Source File
# End Group
# Begin Group "string"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\bigstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\encodedstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\playstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\redirstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\redirstr.h
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\stringpool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\stringtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\url.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\string\utf8.cpp
# End Source File
# End Group
# Begin Group "platform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\platform\guid.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\platform\linux\linux.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\platform\linux.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\platform\platform.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\platform\platform.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\platform\win32.h
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "attrib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\attrib\attrcolor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\attrib\attribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\attrib\attrstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\attrib\cfgitemi.cpp
# End Source File
# End Group
# Begin Group "parse"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\parse\hierarchyparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\parse\node.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\parse\paramparser.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\parse\pathparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\parse\urlparse.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\assert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\critsec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\depend.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\guid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\nsGUID.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\redir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\sourcesink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_cpu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_math.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_mem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_path.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_rand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\thread.cpp
# End Source File
# End Group
# Begin Group "apis"

# PROP Default_Filter ""
# Begin Group "svc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\api_service.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\api_servicei.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\api_servicex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\callbacks\consolecb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\servicei.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svc_enum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svccache.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svcenum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svcenumbyguid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svcenumt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svcmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\waservicefactory.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\waservicefactorybase.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\waservicefactoryi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\waservicefactoryt.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\waservicefactorytsingle.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\waservicefactoryx.cpp
# End Source File
# End Group
# Begin Group "syscb"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\api_syscb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\api_syscbi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\api_syscbx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\cbmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\callbacks\syscb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\callbacks\syscbi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\syscb\callbacks\syscbx.cpp
# End Source File
# End Group
# Begin Group "app"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\api\application\api_application.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\application\api_applicationi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\application\api_applicationx.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\util\inifile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\application\ipcs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\application\pathmgr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\application\version.cpp
# End Source File
# End Group
# Begin Group "timer"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\api\timer\api_timer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\timer\timerapi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\timer\timerclient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\timer\timermul.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\timer\tmultiplex.cpp
# End Source File
# End Group
# Begin Group "linux"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\api\linux\api_linux.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\linux\api_linuxi.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\linux\api_linuxx.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\linux\linuxapi.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "wnd"

# PROP Default_Filter ""
# Begin Group "os"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\osdockwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\osframewnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\osmdichildwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\osmdiclientwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\osmdiparentwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\osstatusbarwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\ostoolbarwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\os\ostooltip.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\backbufferwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\basewnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\draw\bitmap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\draw\blending.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\bltcanvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\canvas.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\clickwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\dockhostwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\dockwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\findobjectcb.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\labelwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\wnds\listwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\popup.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\region.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\rootwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\scbkgwnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\scrollbar.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\bfc\std_wnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\wnd\virtualwnd.cpp
# End Source File
# End Group
# Begin Group "fonts"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\api\font\api_font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\font\fontapi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svcs\svc_font.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\font\win32\truetypefont_win32.cpp
# End Source File
# End Group
# Begin Group "imgldr"

# PROP Default_Filter ""
# Begin Group "jpgload"

# PROP Default_Filter ""
# Begin Group "libjpg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\img\jpgload\jpgdlib\H2v2.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\jpgload\jpgdlib\idct.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\jpgload\jpgdlib\jidctfst.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\jpgload\jpgdlib\jpegdecoder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\wasabi\src\img\jpgload\jpgload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\jpgload\loader_jpg.cpp
# End Source File
# End Group
# Begin Group "pngload"

# PROP Default_Filter ""
# Begin Group "libpng"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNG.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGERROR.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGGET.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGMEM.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGPREAD.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGREAD.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGRIO.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGRTRAN.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGRUTIL.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGSET.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGTEST.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGTRANS.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\pngvcrd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGWIO.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGWRITE.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pnglib\PNGWUTIL.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Group "zlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\ADLER32.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\COMPRESS.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\CRC32.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\DEFLATE.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\GZIO.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\INFBLOCK.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\INFCODES.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\INFFAST.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\INFLATE.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\INFTREES.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\INFUTIL.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\TREES.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\UNCOMPR.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\UNZIP.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\zlib\ZUTIL.C
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\loader.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\pngload\pngload.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\wasabi\src\img\api_imgldr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\imgldr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\img\imgldrapi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wasabi\src\api\service\svcs\svc_imgload.cpp
# End Source File
# End Group
# End Group
# Begin Source File

SOURCE=..\..\wasabi\src\api\apiinit.cpp
# End Source File
# Begin Source File

SOURCE=.\precomp.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\wasabicfg.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Readme.txt
# End Source File
# End Target
# End Project
