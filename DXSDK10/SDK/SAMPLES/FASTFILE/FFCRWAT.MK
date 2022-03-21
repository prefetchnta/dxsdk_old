NAME = ffcreate
EXT = exe

GOALS = $(NAME).$(EXT)

SYS = nt

!ifdef DEBUG
COPT =-DDEBUG -d2
LOPT = debug all
!else
COPT =-oaxt -d1
LOPT =
!endif

OBJS    = ffcreate.obj
          
CFLAGS  =$(COPT) 
LFLAGS  =$(LOPT)

!include ..\..\watsdk.mk

$(NAME).$(EXT): $(OBJS) $(NAME).lnk  
        $(LINK) $(LFLAGS) @$(NAME).lnk
