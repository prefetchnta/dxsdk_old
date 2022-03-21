############################################################################
#
#  Copyright (C) 1995 Microsoft Corporation.  All Rights Reserved.
#
#  File:        watsdk.mk
#  Content:     Rules for building the components of the SDK.
#               For use with WATCOM C/C++
#
############################################################################

!ifndef GSDKROOT
GSDKROOT=..\..\..
!endif

#############################################################################
#
# Set up include & lib path
#
#############################################################################
INCLUDE=$(GSDKROOT)\inc;$(INCLUDE)
LIB=$(GSDKROOT)\lib;$(LIB)

#############################################################################
#
# new suffixes
#
#############################################################################
.EXTENSIONS:
.EXTENSIONS:.exe .dll
.EXTENSIONS:.obj .res
.EXTENSIONS:.c .cpp .asm .h  .rc

#############################################################################
#
# C compiler definitions
#
#############################################################################
CC      =wcl386
CFLAGS  += -W3 -c
!ifndef LOGO
CFLAGS  += -zq
!endif

#############################################################################
#
# Linker definitions
#
#############################################################################
LINK    =wlink 
!ifndef LOGO
LFLAGS  += option quiet 
!endif

#############################################################################
# 
# resource compiler definitions
#
#############################################################################
RCFLAGS += -I..
RCFLAGS += -DWIN32 -DIS_32 -D__WATCOMC__
RC = wrc

#############################################################################
#
# assembler definitions
#
#############################################################################
ASM = ml
AFLAGS  +=-DIS_32 -DWIN32
AFLAGS  +=-W3 -WX -Zd -c -Cx -DMASM6

#############################################################################
#
# librarian definitions
#
#############################################################################
LIBEXE = wlib

#############################################################################
#
# targets
#
#############################################################################

goal:   $(GOALS) .SYMBOLIC
        @%null

.c: ..;..\..\misc

.c.obj:
        $(CC) $(CFLAGS) -bt=NT $[* -fh

.cpp : ..

.cpp.obj:
        $(CC) $(CFLAGS) -bt=NT $[* -fh
   
.asm : ..

.asm.obj:
        $(ASM) $(AFLAGS) $[*.asm
        
.rc : ..

.rc.res:
        $(RC) $(RCFLAGS) -bt=NT -r -fo=$^. $[*.rc
        
$(NAME).lnk : ..\$(MAKENAME) ..\..\watsdk.mk ..\..\watbld.mk
    @%write $(NAME).lnk debug all
    @%append $(NAME).lnk system $(SYS)
    @%append $(NAME).lnk op map
    @%append $(NAME).lnk op st=64k
    @%append $(NAME).lnk name $(NAME).exe
    @for %i in ($(OBJS)) do @%append $(NAME).lnk file %i
    @for %i in ($(LIBS)) do @%append $(NAME).lnk file %i
