# Microsoft Visual C++ Generated NMAKE File, Format Version 2.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

!IF "$(CFG)" == ""
CFG=Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Win32 Release" && "$(CFG)" != "Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "MISC.MAK" CFG="Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

################################################################################
# Begin Project
# PROP Target_Last_Scanned "Win32 Debug"
MTL=MkTypLib.exe
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "retail"
# PROP BASE Intermediate_Dir "retail"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "retail"
# PROP Intermediate_Dir "retail"
OUTDIR=.\retail
INTDIR=.\retail

ALL : .\$(OUTDIR)\misc.dll $(OUTDIR)/MISC.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /YX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /YX /O2 /I "..\..\inc" /I ".\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /YX /O2 /I "..\..\inc" /I ".\include" \
 /D "WIN32" /D "STRICT" /D "NDEBUG"\
 /D "_WINDOWS" /Fp$(OUTDIR)/"MISC.pch" /Fo$(INTDIR)/ /c 
CPP_OBJS=.\retail/
# ADD BASE RSC /d "NDEBUG"
# ADD RSC /d "NDEBUG"
BSC32=bscmake.exe
BSC32_SBRS= \
    
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MISC.bsc" 

$(OUTDIR)/MISC.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DLL /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DLL /MACHINE:I386 /OUT:"$(OUTDIR)\"misc.dll""
# SUBTRACT LINK32 /INCREMENTAL:yes
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO\
 /SUBSYSTEM:windows,4.0 /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"MISC.pdb" /MACHINE:I386\
 /DEF:".\MISC.DEF" /OUT:$(OUTDIR)\""misc.dll"" /IMPLIB:$(OUTDIR)/"MISC.lib" 
DEF_FILE=.\MISC.DEF
LINK32_OBJS= \
    $(INTDIR)/CGINPUT.OBJ \
    $(INTDIR)/CGMISC.OBJ

.\$(OUTDIR)\misc.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
    copy $(OUTDIR)\misc.dll misc.dll

!ELSEIF  "$(CFG)" == "Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "debug"
# PROP BASE Intermediate_Dir "debug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
OUTDIR=.\debug
INTDIR=.\debug

ALL : $(OUTDIR)\misc.dll $(OUTDIR)/MISC.bsc

$(OUTDIR) : 
    if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE CPP /nologo /MT /W3 /GX /Zi /YX /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /c
# ADD CPP /nologo /MT /W3 /GX /Zi /YX /Od /I "..\..\inc" /I ".\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MT /W3 /GX /Zi /YX /Od /I "..\..\inc" /I ".\include" \
 /D "WIN32" /D "STRICT" /D "_DEBUG"\
 /D "_WINDOWS" /Fp$(OUTDIR)/"MISC.pch" /Fo$(INTDIR)/ /Fd$(OUTDIR)/"MISC.pdb" /c 
CPP_OBJS=.\debug/
# ADD BASE RSC /d "_DEBUG"
# ADD RSC /d "_DEBUG"
BSC32=bscmake.exe
BSC32_SBRS= \
    
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o$(OUTDIR)/"MISC.bsc" 

$(OUTDIR)/MISC.bsc : $(OUTDIR)  $(BSC32_SBRS)
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DLL /DEBUG /MACHINE:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO /SUBSYSTEM:windows,4.0 /DLL /INCREMENTAL:no /DEBUG /MACHINE:I386 /OUT:"$(OUTDIR)\"misc.dll""
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib winmm.lib /NOLOGO\
 /SUBSYSTEM:windows,4.0 /DLL /INCREMENTAL:no /PDB:$(OUTDIR)/"MISC.pdb" /DEBUG\
 /MACHINE:I386 /DEF:".\MISC.DEF" /OUT:$(OUTDIR)\""misc.dll""\
 /IMPLIB:$(OUTDIR)/"MISC.lib" 
DEF_FILE=.\MISC.DEF
LINK32_OBJS= \
    $(INTDIR)/CGINPUT.OBJ \
    $(INTDIR)/CGMISC.OBJ

$(OUTDIR)\misc.dll : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
    copy $(OUTDIR)\misc.dll misc.dll

!ENDIF 

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

SOURCE=.\MISC.DEF
# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGINPUT.CPP
DEP_CGINP=\
    .\CGINPUT.H

$(INTDIR)/CGINPUT.OBJ :  $(SOURCE)  $(DEP_CGINP) $(INTDIR)

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGMISC.CPP
DEP_CGMIS=\
    .\CGCHDLL.H\
    .\CGCHAR.H\
    .\CGTIMER.H\
    .\CGINPUT.H\
    .\CGUPDATE.H\
    .\CGACTION.H\
    .\CGGRAPH.H\
    .\cgremote.h\
    .\INCLUDE\LINKLIST.H\
    .\CGSPRITE.H\
    .\cgsound.h\
    .\CGLEVEL.H\
    .\CGSCREEN.H\
    .\cgremque.h\
    .\CGBITBUF.H\
    .\CGDLIST.H\
    .\INCLUDE\CGDIB.H

$(INTDIR)/CGMISC.OBJ :  $(SOURCE)  $(DEP_CGMIS) $(INTDIR)

# End Source File
# End Group
# End Project
################################################################################
