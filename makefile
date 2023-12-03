# Example 13
EXE=final
DEBUG=debug
MAPGEN=mapGenerator

# Main target
all: $(EXE) $(DEBUG) $(MAPGEN)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES)
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) $(DEBUG) $(MAPGEN) *.o *.a
endif

# Dependencies
final.o: final.c CSCIx229.h igor.h
debug.o: debug.c CSCIx229.h igor.h
mapGenerator.o: mapGenerator.c CSCIx229.h igor.h

# Objects
house.o: house.c CSCIx229.h igor.h
ground.o: ground.c CSCIx229.h igor.h
helicopter.o: helicopter.c CSCIx229.h igor.h
movement.o: movement.c CSCIx229.h igor.h
light.o: light.c CSCIx229.h igor.h
tree.o: tree.c CSCIx229.h igor.h
weapon.o: weapon.c CSCIx229.h igor.h
dem.o: dem.c CSCIx229.h igor.h
shader.o: shader.c CSCIx229.h igor.h

# CSCI Lib
fatal.o: fatal.c CSCIx229.h
errcheck.o: errcheck.c CSCIx229.h
print.o: print.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
loadobj.o: loadobj.c CSCIx229.h
projection.o: projection.c CSCIx229.h

#  Create archive
CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
	ar -rcs $@ $^

#  Create archive for my files
igor.a:helicopter.o house.o ground.o movement.o light.o tree.o weapon.o dem.o shader.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG)  $<
.cpp.o:
	g++ -c $(CFLG)  $<

#  Link
final:final.o   CSCIx229.a   igor.a
	gcc $(CFLG) -o $@ $^  $(LIBS)

#  Link debugger
debug:debug.o   CSCIx229.a   igor.a
	gcc $(CFLG) -o $@ $^  $(LIBS)

# Map Generator
mapGenerator:mapGenerator.o   CSCIx229.a   igor.a
	gcc $(CFLG) -o $@ $^  $(LIBS)

#  Clean
clean:
	$(CLEAN)
