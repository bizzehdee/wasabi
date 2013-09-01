# Microsoft Developer Studio Project File - Name="jpgsave" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=jpgsave - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpgsave.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpgsave.mak" CFG="jpgsave - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpgsave - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "jpgsave - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "$/Studio/jpgsave"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpgsave - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgsave_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /Zi /O2 /I "../studio/wa3cfg" /I "../" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgsave_EXPORTS" /D "COMMON_STATIC_LINK" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /pdb:none /debug /machine:I386 /out:"../studio/Wacs/wasabi.system/img/jpgsave.wac"

!ELSEIF  "$(CFG)" == "jpgsave - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgsave_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "../studio/wa3cfg" /I "../" /D "_DEBUG" /D "WACLIENT_NOICONSUPPORT" /D "REAL_STDIO" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "jpgsave_EXPORTS" /D "COMMON_STATIC_LINK" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../studio/Wacs/wasabi.system/img/jpgsave.wac" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "jpgsave - Win32 Release"
# Name "jpgsave - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "cjpeg"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libjpeg\jcapimin.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcapistd.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jccoefct.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jccolor.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcdctmgr.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jchuff.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jchuff.h
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcinit.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcmainct.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcmarker.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcmaster.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcomapi.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcparam.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcphuff.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcprepct.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jcsample.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jctrans.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jerror.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jfdctflt.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jfdctfst.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jfdctint.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jidctflt.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jidctfst.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jidctint.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jidctred.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jmemansi.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jmemmgr.c
# End Source File
# Begin Source File

SOURCE=.\libjpeg\jutils.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\jpgsave.cpp
# End Source File
# Begin Source File

SOURCE=..\precomp.cpp
# ADD CPP /Yc"precomp.h"
# End Source File
# Begin Source File

SOURCE=.\saver.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\jpgsave.h
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
