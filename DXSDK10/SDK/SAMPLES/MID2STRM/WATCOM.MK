NAME = mid2strm
EXT = exe

GOALS = $(NAME).$(EXT)

OBJS    =  mid2strm.obj
          
SYS = nt

!ifdef DEBUG 
COPT =-DDEBUG -d2
LOPT = debug all
ROPT =-DDEBUG
!else
COPT =-oaxt -d1
LOPT =
ROPT =
!endif

CFLAGS  =$(COPT)
LFLAGS  =$(LOPT)
RCFLAGS =$(ROPT)

!include ..\..\watsdk.mk

$(NAME).$(EXT): $(OBJS) $(NAME).lnk  
        $(LINK) $(LFLAGS) @$(NAME).lnk
