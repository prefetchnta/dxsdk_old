NAME = palette
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =..\..\..\lib\ddraw.lbw 

SYS = nt_win

OBJS    =  palette.obj lbprintf.obj
          
!ifdef DEBUG 
COPT =-DDEBUG -d2
LOPT = debug all
ROPT =-DDEBUG
!else
COPT =-oaxt -d1
LOPT =
ROPT =
!endif

DEF = $(NAME).def
RES = $(NAME).res 

CFLAGS  =$(COPT) -I..\..\misc
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

!include ..\..\watsdk.mk

$(NAME).$(EXT): $(OBJS) $(NAME).lnk  $(RES)
        $(LINK) $(LFLAGS) @$(NAME).lnk
        $(RC) $(RES)
