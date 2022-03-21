NAME = fastfile
EXT = lib

GOALS = $(NAME).$(EXT) 

OBJS=fastfile.obj
        
!ifdef DEBUG
COPT =-DDEBUG -d2
!else
COPT =-oaxt -d1
!endif

CFLAGS  =$(COPT) 

!include ..\..\watsdk.mk

$(NAME).$(EXT): $(OBJS) $(NAME).lbc
        @$(LIBEXE) -n $(NAME).$(EXT) @$(NAME).lbc

$(NAME).lbc: ..\ffwat.mk
    @%write $(NAME).lbc
    @for %i in ($(OBJS)) do @%append $(NAME).lbc -+%i
