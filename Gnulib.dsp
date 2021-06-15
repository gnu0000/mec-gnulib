# Microsoft Developer Studio Project File - Name="GnuLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GnuLib - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Gnulib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Gnulib.mak" CFG="GnuLib - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GnuLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GnuLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GnuLib - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "$(ITIINC)" /D "NDEBUG" /D "WIN32" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Special Build Tool
OutDir=.\Release
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=if not defined ITIINC echo You need to set the ITIINC environment variable! 	if not defined ITILIB echo You need to set the ITILIB environment variable! 	if defined ITIINC copy $(ProjDir)\gnu*.h %ITIINC%\ 	if defined ITILIB copy $(OutDir)\*.lib %ITILIB%\ 
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GnuLib - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "$(ITIINC)" /D "_DEBUG" /D "WIN32" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Debug\Gnulib_Debug.lib"
# Begin Special Build Tool
OutDir=.\Debug
ProjDir=.
SOURCE="$(InputPath)"
PostBuild_Cmds=if not defined ITIINC echo You need to set the ITIINC environment variable! 	if not defined ITILIB echo You need to set the ITILIB environment variable! 	if defined ITIINC copy $(ProjDir)\gnu*.h %ITIINC%\ 	if defined ITILIB copy $(OutDir)\*.lib %ITILIB%\ 
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "GnuLib - Win32 Release"
# Name "GnuLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Arg0.cpp
# End Source File
# Begin Source File

SOURCE=.\Arg1.cpp
# End Source File
# Begin Source File

SOURCE=.\Cfg0.cpp
# End Source File
# Begin Source File

SOURCE=.\Crc0.cpp
# End Source File
# Begin Source File

SOURCE=.\Des0.cpp
# End Source File
# Begin Source File

SOURCE=.\Dir0.cpp
# End Source File
# Begin Source File

SOURCE=.\Dir1.cpp
# End Source File
# Begin Source File

SOURCE=.\Dir2.cpp
# End Source File
# Begin Source File

SOURCE=.\Dir3.cpp
# End Source File
# Begin Source File

SOURCE=.\Dir4.cpp
# End Source File
# Begin Source File

SOURCE=.\Dir5.cpp
# End Source File
# Begin Source File

SOURCE=.\Dir6.cpp
# End Source File
# Begin Source File

SOURCE=.\dir7.cpp
# End Source File
# Begin Source File

SOURCE=.\Fil0.cpp
# End Source File
# Begin Source File

SOURCE=.\Fil1.cpp
# End Source File
# Begin Source File

SOURCE=.\Fil2.cpp
# End Source File
# Begin Source File

SOURCE=.\Fil3.cpp
# End Source File
# Begin Source File

SOURCE=.\Fil4.cpp
# End Source File
# Begin Source File

SOURCE=.\Fil5.cpp
# End Source File
# Begin Source File

SOURCE=.\Grd0.cpp
# End Source File
# Begin Source File

SOURCE=.\Grd1.cpp
# End Source File
# Begin Source File

SOURCE=.\Kbd0.cpp
# End Source File
# Begin Source File

SOURCE=.\Kbd1.cpp
# End Source File
# Begin Source File

SOURCE=.\Mem0.cpp
# End Source File
# Begin Source File

SOURCE=.\Mem1.cpp
# End Source File
# Begin Source File

SOURCE=.\Mem2.cpp
# End Source File
# Begin Source File

SOURCE=.\Mem3.cpp
# End Source File
# Begin Source File

SOURCE=.\Mem4.cpp
# End Source File
# Begin Source File

SOURCE=.\Msc0.cpp
# End Source File
# Begin Source File

SOURCE=.\Msc1.cpp
# End Source File
# Begin Source File

SOURCE=.\Msc2.cpp
# End Source File
# Begin Source File

SOURCE=.\Msc3.cpp
# End Source File
# Begin Source File

SOURCE=.\Msc4.cpp
# End Source File
# Begin Source File

SOURCE=.\Msc5.cpp
# End Source File
# Begin Source File

SOURCE=.\Msc6.cpp
# End Source File
# Begin Source File

SOURCE=.\Mth0.cpp
# End Source File
# Begin Source File

SOURCE=.\Mth1.cpp
# End Source File
# Begin Source File

SOURCE=.\Oldzip.cpp
# End Source File
# Begin Source File

SOURCE=.\rc40.cpp
# End Source File
# Begin Source File

SOURCE=.\Reg0.cpp
# End Source File
# Begin Source File

SOURCE=.\Res0.cpp
# End Source File
# Begin Source File

SOURCE=.\Res1.cpp
# End Source File
# Begin Source File

SOURCE=.\Res2.cpp
# End Source File
# Begin Source File

SOURCE=.\Res3.cpp
# End Source File
# Begin Source File

SOURCE=.\Res4.cpp
# End Source File
# Begin Source File

SOURCE=.\Scr0.cpp
# End Source File
# Begin Source File

SOURCE=.\Scr1.cpp
# End Source File
# Begin Source File

SOURCE=.\Scr2.cpp
# End Source File
# Begin Source File

SOURCE=.\Scr3.cpp
# End Source File
# Begin Source File

SOURCE=.\Scr4.cpp
# End Source File
# Begin Source File

SOURCE=.\Str0.cpp
# End Source File
# Begin Source File

SOURCE=.\Str1.cpp
# End Source File
# Begin Source File

SOURCE=.\Str2.cpp
# End Source File
# Begin Source File

SOURCE=.\Str3.cpp
# End Source File
# Begin Source File

SOURCE=.\Str4.cpp
# End Source File
# Begin Source File

SOURCE=.\Tim0.cpp
# End Source File
# Begin Source File

SOURCE=.\Var0.cpp
# End Source File
# Begin Source File

SOURCE=.\Win0.cpp
# End Source File
# Begin Source File

SOURCE=.\Win1.cpp
# End Source File
# Begin Source File

SOURCE=.\Win2.cpp
# End Source File
# Begin Source File

SOURCE=.\Win3.cpp
# End Source File
# Begin Source File

SOURCE=.\Win4.cpp
# End Source File
# Begin Source File

SOURCE=.\Win5.cpp
# End Source File
# Begin Source File

SOURCE=.\Win7.cpp
# End Source File
# Begin Source File

SOURCE=.\Win8.cpp
# End Source File
# Begin Source File

SOURCE=.\Win9.cpp
# End Source File
# Begin Source File

SOURCE=.\Wina.cpp
# End Source File
# Begin Source File

SOURCE=.\Winb.cpp
# End Source File
# Begin Source File

SOURCE=.\Winc.cpp
# End Source File
# Begin Source File

SOURCE=.\Wind.cpp
# End Source File
# Begin Source File

SOURCE=.\Wine.cpp
# End Source File
# Begin Source File

SOURCE=.\Winf.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip0.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip1.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip2.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip3.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip4.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip5.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip6.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip7.cpp
# End Source File
# Begin Source File

SOURCE=.\Zip8.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Arg.h
# End Source File
# Begin Source File

SOURCE=.\Gnuarg.h
# End Source File
# Begin Source File

SOURCE=.\Gnucfg.h
# End Source File
# Begin Source File

SOURCE=.\Gnucrc.h
# End Source File
# Begin Source File

SOURCE=.\Gnudes.h
# End Source File
# Begin Source File

SOURCE=.\Gnudir.h
# End Source File
# Begin Source File

SOURCE=.\Gnufile.h
# End Source File
# Begin Source File

SOURCE=.\Gnugrid.h
# End Source File
# Begin Source File

SOURCE=.\Gnukbd.h
# End Source File
# Begin Source File

SOURCE=.\Gnumath.h
# End Source File
# Begin Source File

SOURCE=.\Gnumem.h
# End Source File
# Begin Source File

SOURCE=.\Gnumisc.h
# End Source File
# Begin Source File

SOURCE=.\gnurc4.h
# End Source File
# Begin Source File

SOURCE=.\Gnureg.h
# End Source File
# Begin Source File

SOURCE=.\Gnures.h
# End Source File
# Begin Source File

SOURCE=.\Gnuscr.h
# End Source File
# Begin Source File

SOURCE=.\Gnustr.h
# End Source File
# Begin Source File

SOURCE=.\Gnutime.h
# End Source File
# Begin Source File

SOURCE=.\Gnutype.h
# End Source File
# Begin Source File

SOURCE=.\Gnuvar.h
# End Source File
# Begin Source File

SOURCE=.\Gnuwin.h
# End Source File
# Begin Source File

SOURCE=.\Gnuzip.h
# End Source File
# Begin Source File

SOURCE=.\Kbd.h
# End Source File
# Begin Source File

SOURCE=.\Mem.h
# End Source File
# Begin Source File

SOURCE=.\Mth.h
# End Source File
# Begin Source File

SOURCE=.\Res.h
# End Source File
# Begin Source File

SOURCE=.\Zip.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\Implode.obj
# End Source File
# Begin Source File

SOURCE=.\Explode.obj
# End Source File
# End Target
# End Project
