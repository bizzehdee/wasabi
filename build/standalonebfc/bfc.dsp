# Microsoft Developer Studio Project File - Name="bfc" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=bfc - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "bfc.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "bfc.mak" CFG="bfc - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "bfc - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "bfc - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "bfc - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "bfc - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "bfc - Win32 Release"
# Name "bfc - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "file"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\bfc\file\filemon.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\filename.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\readdir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\recursedir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\splitpath.c
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\wildcharsenum.cpp
# End Source File
# End Group
# Begin Group "util"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\bfc\util\findopenrect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\inifile.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\profiler.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\timefmt.cpp
# End Source File
# End Group
# Begin Group "string"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\bfc\string\bigstring.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringMap.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringPair.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringpool.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringtable.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\url.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\utf8.cpp
# End Source File
# End Group
# Begin Group "platform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\bfc\platform\platform.cpp
# End Source File
# End Group
# Begin Group "attrib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrcolor.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\cfgitemi.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\src\bfc\assert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\bigcounter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\critsec.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\depend.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\foreach.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\freelist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\guid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\loadlib.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\memblock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\mutex.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\nsguid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\parse\pathparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\ptrlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\rect.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\redir.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\sharedmem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\sourcesink.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\stack.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_cpu.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_file.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_math.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_mem.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_path.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_rand.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_wnd.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\thread.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\threadwait.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\bfc\assert.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrbool.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrcb.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrcolor.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrfloat.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrfn.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrhandler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attribs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attribute.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrint.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\attrstr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\bigstring.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\bitlist.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\cfgitem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\attrib\cfgitemi.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\circbuf.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\common.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\compare.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\critsec.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\depend.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\depview.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\dispatch.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\filemon.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\filename.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\findopenrect.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\foreach.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\freelist.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\guid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\platform\guid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\inifile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\loadlib.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\map.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\memblock.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\multimap.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\mutex.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\named.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\nsguid.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\pair.h
# End Source File
# Begin Source File

SOURCE=..\..\src\parse\pathparse.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\platform\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\popup.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\profile.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\profiler.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\ptrlist.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\readdir.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\recursedir.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\redir.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\reentryfilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\sharedmem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\sourcesink.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\splitpath.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\stack.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_cpu.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_file.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_math.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_mem.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_mkncc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_path.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_rand.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_string.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\std_wnd.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\string.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringdict.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringMap.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringPair.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringpool.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\stringtable.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\thread.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\util\timefmt.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\tlist.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\tmpnamestr.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\url.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\string\utf8.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\file\wildcharsenum.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\platform\win32\win32.h
# End Source File
# Begin Source File

SOURCE=..\..\src\bfc\platform\winapi.h
# End Source File
# End Group
# End Target
# End Project
