# Microsoft Developer Studio Project File - Name="pnglib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=pnglib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "pnglib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "pnglib.mak" CFG="pnglib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "pnglib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "pnglib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "pnglib - Win32 Release"

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
# ADD CPP /nologo /G5 /W3 /GX /O2 /Ob2 /I "../libtiff" /I "../jpeglib" /I "../zlib" /I "../fnlib" /I "../libpaint" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "pnglib - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../libtiff" /I "../jpeglib" /I "../zlib" /I "../fnlib" /I "../libpaint" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "pnglib - Win32 Release"
# Name "pnglib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\PNG.C
# End Source File
# Begin Source File

SOURCE=.\PNGERROR.C
# End Source File
# Begin Source File

SOURCE=.\PNGGET.C
# End Source File
# Begin Source File

SOURCE=.\PNGMEM.C
# End Source File
# Begin Source File

SOURCE=.\PNGPREAD.C
# End Source File
# Begin Source File

SOURCE=.\PNGREAD.C
# End Source File
# Begin Source File

SOURCE=.\PNGRIO.C
# End Source File
# Begin Source File

SOURCE=.\PNGRTRAN.C
# End Source File
# Begin Source File

SOURCE=.\PNGRUTIL.C
# End Source File
# Begin Source File

SOURCE=.\PNGSET.C
# End Source File
# Begin Source File

SOURCE=.\PNGTEST.C
# End Source File
# Begin Source File

SOURCE=.\PNGTRANS.C
# End Source File
# Begin Source File

SOURCE=.\PNGWIO.C
# End Source File
# Begin Source File

SOURCE=.\PNGWRITE.C
# End Source File
# Begin Source File

SOURCE=.\PNGWTRAN.C
# End Source File
# Begin Source File

SOURCE=.\PNGWUTIL.C
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\PNG.H
# End Source File
# Begin Source File

SOURCE=.\PNGCONF.H
# End Source File
# End Group
# End Target
# End Project
