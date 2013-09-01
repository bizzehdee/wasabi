# Microsoft Developer Studio Project File - Name="jpgload" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=jpgload - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpgload_wac.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpgload_wac.mak" CFG="jpgload - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpgload - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jpgload - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/Studio/jpgload"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpgload - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgload_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "../../../wasabicfg/wa5" /I "../../../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgload_EXPORTS" /Yu"precomp.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /machine:I386 /out:"c:\program files\winamp\plugins\freeform\wacs\jpgload\jpgload.wac"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "jpgload - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgload_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../../../wasabicfg/wa5" /I "../../../" /D "_DEBUG" /D "WACLIENT_NOICONSUPPORT" /D "REAL_STDIO" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgload_EXPORTS" /Yu"precomp.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"c:\program files\winamp\plugins\freeform\wacs\jpgload\jpgload.wac" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "jpgload - Win32 Release"
# Name "jpgload - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "jpgdlib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\jpgdlib\H2v2.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\jpgdlib\idct.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\jpgdlib\jidctfst.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\jpgdlib\jpegdecoder.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\jpgdlib\jpegdecoder.h
# End Source File
# Begin Source File

SOURCE=.\jpgdlib\jpegdecoder.inl
# End Source File
# Begin Source File

SOURCE=.\jpgdlib\main.h
# End Source File
# End Group
# Begin Group "wac"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\bfc\assert.cpp
# End Source File
# Begin Source File

SOURCE=..\..\config\items\attribute.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wnd\blending.cpp
# End Source File
# Begin Source File

SOURCE=..\..\config\items\cfgitemi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\depend.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\string\encodedstr.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\foreach.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\memblock.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\nsguid.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\parse\pathparse.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\ptrlist.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\std.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\bfc\string\string.cpp
# End Source File
# Begin Source File

SOURCE=..\..\service\svc_enum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\service\svcs\svc_imgload.cpp
# End Source File
# Begin Source File

SOURCE=..\..\service\svcenum.cpp
# End Source File
# Begin Source File

SOURCE=..\..\wac\waclient.cpp
# End Source File
# Begin Source File

SOURCE=..\..\service\waservicefactoryi.cpp
# End Source File
# Begin Source File

SOURCE=..\..\service\waservicefactoryx.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\jpgload.cpp
# End Source File
# Begin Source File

SOURCE=.\loader_jpg.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\..\gen_ff\precomp.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# Begin Source File

SOURCE=..\..\..\wasabicfg\wa5\wasabicfg.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\jpgload.h
# End Source File
# Begin Source File

SOURCE=.\loader.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
