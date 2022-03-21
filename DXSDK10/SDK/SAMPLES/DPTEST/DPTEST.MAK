# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Debug" && "$(CFG)" != "Win32 Release"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "dptest.mak" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : $(OUTDIR)/dptest.exe $(OUTDIR)/dptest.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MD /W3 /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /Zi /YX"stdafx.h" /Od /I "..\..\inc" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /c
CPP_PROJ=/nologo /MD /W3 /GX /Zi /YX"stdafx.h" /Od /I "..\..\inc" /D\
 "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"dptest.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dptest.pdb" /c 
CPP_OBJS=.\WinDebug/
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dptest.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dptest.bsc" 
BSC32_SBRS= \
    $(INTDIR)/stdafx.sbr \
    $(INTDIR)/dptest.sbr \
    $(INTDIR)/mainfrm.sbr \
    $(INTDIR)/dptedoc.sbr \
    $(INTDIR)/dptevw.sbr \
    $(INTDIR)/newentdg.sbr \
    $(INTDIR)/getentdg.sbr \
    $(INTDIR)/openchan.sbr \
    $(INTDIR)/senddlg.sbr \
    $(INTDIR)/recvdlg.sbr \
    $(INTDIR)/destroyp.sbr \
    $(INTDIR)/opendlg.sbr

$(OUTDIR)/dptest.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 ..\..\lib\dplay.lib /NOLOGO /SUBSYSTEM:windows /DEBUG /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=..\..\lib\dplay.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:yes\
 /PDB:$(OUTDIR)/"dptest.pdb" /DEBUG /MACHINE:I386 /OUT:"WinDebug\dptest.exe" 
DEF_FILE=
LINK32_OBJS= \
    $(INTDIR)/stdafx.obj \
    $(INTDIR)/dptest.obj \
    $(INTDIR)/mainfrm.obj \
    $(INTDIR)/dptedoc.obj \
    $(INTDIR)/dptevw.obj \
    $(INTDIR)/dptest.res \
    $(INTDIR)/newentdg.obj \
    $(INTDIR)/getentdg.obj \
    $(INTDIR)/openchan.obj \
    $(INTDIR)/senddlg.obj \
    $(INTDIR)/recvdlg.obj \
    $(INTDIR)/destroyp.obj \
    $(INTDIR)/opendlg.obj

$(OUTDIR)/dptest.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : $(OUTDIR)/dptest.exe $(OUTDIR)/dptest.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /YX"stdafx.h" /O2 /I "..\..\inc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /c
CPP_PROJ=/nologo /MD /W3 /GX /YX"stdafx.h" /O2 /I "..\..\inc" /D "NDEBUG"\
 /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR$(INTDIR)/\
 /Fp$(OUTDIR)/"dptest.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\WinRel/
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo$(INTDIR)/"dptest.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"dptest.bsc" 
BSC32_SBRS= \
    $(INTDIR)/stdafx.sbr \
    $(INTDIR)/dptest.sbr \
    $(INTDIR)/mainfrm.sbr \
    $(INTDIR)/dptedoc.sbr \
    $(INTDIR)/dptevw.sbr \
    $(INTDIR)/newentdg.sbr \
    $(INTDIR)/getentdg.sbr \
    $(INTDIR)/openchan.sbr \
    $(INTDIR)/senddlg.sbr \
    $(INTDIR)/recvdlg.sbr \
    $(INTDIR)/destroyp.sbr \
    $(INTDIR)/opendlg.sbr

$(OUTDIR)/dptest.bsc : $(OUTDIR)  $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
# ADD BASE LINK32 /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT BASE LINK32 /PDB:none
# ADD LINK32 ..\..\lib\dplay.lib /NOLOGO /SUBSYSTEM:windows /MACHINE:I386
# SUBTRACT LINK32 /PDB:none
LINK32_FLAGS=..\..\lib\dplay.lib /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"dptest.pdb" /MACHINE:I386 /OUT:"WinRel\dptest.exe" 
DEF_FILE=
LINK32_OBJS= \
    $(INTDIR)/stdafx.obj \
    $(INTDIR)/dptest.obj \
    $(INTDIR)/mainfrm.obj \
    $(INTDIR)/dptedoc.obj \
    $(INTDIR)/dptevw.obj \
    $(INTDIR)/dptest.res \
    $(INTDIR)/newentdg.obj \
    $(INTDIR)/getentdg.obj \
    $(INTDIR)/openchan.obj \
    $(INTDIR)/senddlg.obj \
    $(INTDIR)/recvdlg.obj \
    $(INTDIR)/destroyp.obj \
    $(INTDIR)/opendlg.obj

$(OUTDIR)/dptest.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

MTL_PROJ=

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Group "Source Files"

################################################################################
# Begin Source File

SOURCE=.\stdafx.cpp
DEP_STDAF=\
    .\stdafx.h

!IF  "$(CFG)" == "Win32 Debug"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /Zi /Od /I "..\..\inc" /D "_DEBUG" /D "WIN32"\
 /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dptest.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"dptest.pdb" /c  $(SOURCE) 

!ELSEIF  "$(CFG)" == "Win32 Release"

# ADD BASE CPP /Yc"stdafx.h"
# ADD CPP /Yc"stdafx.h"

$(INTDIR)/stdafx.obj :  $(SOURCE)  $(DEP_STDAF) $(INTDIR)
   $(CPP) /nologo /MD /W3 /GX /O2 /I "..\..\inc" /D "NDEBUG" /D "WIN32" /D\
 "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR$(INTDIR)/ /Fp$(OUTDIR)/"dptest.pch"\
 /Yc"stdafx.h" /Fo$(INTDIR)/ /c  $(SOURCE) 

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dptest.cpp
DEP_DPTE=\
    .\stdafx.h\
    .\dptest.h\
    .\mainfrm.h\
    .\dptedoc.h\
    .\dptevw.h

$(INTDIR)/dptest.obj :  $(SOURCE)  $(DEP_DPTE) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\mainfrm.cpp
DEP_MAINF=\
    .\stdafx.h\
    ..\..\inc\dplay.h\
    .\dptest.h\
    .\newentdg.h\
    .\getentdg.h\
    .\openchan.h\
    .\senddlg.h\
    .\recvdlg.h\
    .\destroyp.h\
    .\mainfrm.h\
    .\util.h

$(INTDIR)/mainfrm.obj :  $(SOURCE)  $(DEP_MAINF) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dptedoc.cpp
DEP_DPTED=\
    .\stdafx.h\
    .\dptest.h\
    .\dptedoc.h

$(INTDIR)/dptedoc.obj :  $(SOURCE)  $(DEP_DPTED) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dptevw.cpp
DEP_DPTEV=\
    .\stdafx.h\
    .\dptest.h\
    .\dptedoc.h\
    .\dptevw.h

$(INTDIR)/dptevw.obj :  $(SOURCE)  $(DEP_DPTEV) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\dptest.rc
DEP_DPTES=\
    .\res\dptest.ico\
    .\res\dptest.rc2

$(INTDIR)/dptest.res :  $(SOURCE)  $(DEP_DPTES) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\readme.txt
# End Source File
################################################################################
# Begin Source File

SOURCE=.\newentdg.cpp
DEP_NEWEN=\
    .\stdafx.h\
    .\dptest.h\
    .\newentdg.h

$(INTDIR)/newentdg.obj :  $(SOURCE)  $(DEP_NEWEN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\getentdg.cpp
DEP_GETEN=\
    .\stdafx.h\
    .\dptest.h\
    ..\..\inc\dplay.h\
    .\util.h\
    .\getentdg.h

$(INTDIR)/getentdg.obj :  $(SOURCE)  $(DEP_GETEN) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\openchan.cpp
DEP_OPENC=\
    .\stdafx.h\
    .\dptest.h\
    .\openchan.h

$(INTDIR)/openchan.obj :  $(SOURCE)  $(DEP_OPENC) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\senddlg.cpp
DEP_SENDD=\
    .\stdafx.h\
    .\dptest.h\
    .\senddlg.h

$(INTDIR)/senddlg.obj :  $(SOURCE)  $(DEP_SENDD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\recvdlg.cpp
DEP_RECVD=\
    .\stdafx.h\
    .\dptest.h\
    .\recvdlg.h

$(INTDIR)/recvdlg.obj :  $(SOURCE)  $(DEP_RECVD) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\destroyp.cpp
DEP_DESTR=\
    .\stdafx.h\
    .\dptest.h\
    .\destroyp.h

$(INTDIR)/destroyp.obj :  $(SOURCE)  $(DEP_DESTR) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\opendlg.cpp
DEP_OPEND=\
    .\stdafx.h\
    .\dptest.h\
    .\opendlg.h

$(INTDIR)/opendlg.obj :  $(SOURCE)  $(DEP_OPEND) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################

