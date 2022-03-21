NAME = ddex5
EXT = exe

GOALS = $(NAME).$(EXT)

LIBS    =..\..\..\lib\ddraw.lbw 

OBJS    =  ddex5.obj
          
SYS = nt_win

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

CFLAGS  =$(COPT)
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

!include ..\..\watsdk.mk

$(NAME).$(EXT): $(OBJS) $(NAME).lnk  $(RES)
        $(LINK) $(LFLAGS) @$(NAME).lnk
        $(RC) $(RES)
