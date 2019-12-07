# Makefile Partially Taken from CSCI 4229/5229 from University of Colorado Boulder
# Modified to take SDL stuff (hopefully everything works on Linux and Windows, but not tested on either yet.)
# Credit goes to Williem (Vlakkies) Schreuder for creating a base to start with this. 
all: selfdraw

# These are for if you are using SDL.
# Hopefully everything here works on Linux and Windows?
OSXGLNK=SDL2 -framework SDL2_image
LNXGLNK=-lSDL2 -lSDL2image
MGWGLNK=-lSDL2 -lSLD2image
#OSXINCLUDE=-I /Library/Frameworks/SDL2.framework/Headers
OSXINCLUDE=-F /Library/Frameworks

#  MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=$(MGWGLNK) -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations $(OSXINCLUDE)
LIBS=-framework $(OSXGLNK) -framework OpenGL $(OSXINCLUDE)
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=$(LNXGLNK) -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f game *.o *.a
endif

# Compile rules
.c.o:
	gcc -o $@ -c $(CFLG) $<
.cpp.o:
	g++ -std=c++11 -o $@ -c $(CFLG) $<

selfdraw: main.o image.o lines.o
	g++ -std=c++11 -O3 -o $@ $^ $(LIBS) 

clean:
	rm -f *.o *.a selfdraw
