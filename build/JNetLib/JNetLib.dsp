# Microsoft Developer Studio Project File - Name="JNetLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=JNetLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "JNetLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "JNetLib.mak" CFG="JNetLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "JNetLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "JNetLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "JNetLib - Win32 Release"

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

!ELSEIF  "$(CFG)" == "JNetLib - Win32 Debug"

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

# Name "JNetLib - Win32 Release"
# Name "JNetLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\net\jnetlib\asyncdns.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\connection.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\httpget.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\httpserv.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\listen.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\util.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\webserver.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\net\jnetlib\asyncdns.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\connection.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\httpget.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\httpserv.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\jnetlib.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\listen.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\netinc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\util.h
# End Source File
# Begin Source File

SOURCE=..\..\src\net\jnetlib\webserver.h
# End Source File
# End Group
# End Target
# End Project
