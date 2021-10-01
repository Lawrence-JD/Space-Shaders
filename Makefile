
PROGS=../../progs

CXX=g++
CXXFLAGS+=-Werror -Wall -Wdouble-promotion -Wfloat-conversion -g -I/usr/include/SDL2 -I$(PROGS)/gl -I$(PROGS)/math3d 
CXXFLAGS+=-I$(PROGS)/image -I$(PROGS)/miniz

CXXFLAGS += -O0

LDFLAGS=-lpng -lz -lturbojpeg -lSDL2 -lSDL2_mixer -ldl 

main: main.o statics.o glfuncs.o miniz_tinfl.o
	$(CXX) main.o statics.o glfuncs.o miniz_tinfl.o -o main $(LDFLAGS)

main.o: main.cpp $(wildcard *.h)
	$(CXX) $(CXXFLAGS) main.cpp -c
	
statics.o: statics.cpp $(wildcard *.h)
	$(CXX) $(CXXFLAGS) statics.cpp -c

glfuncs.o: $(PROGS)/gl/glfuncs.cpp
	$(CXX) $(CXXFLAGS) -o glfuncs.o -c $(PROGS)/gl/glfuncs.cpp

miniz_tinfl.o: $(PROGS)/miniz/miniz_tinfl.c
	$(CXX) $(CXXFLAGS) -c $(PROGS)/miniz/miniz_tinfl.c
	
clean:
	-/bin/rm $(wildcard *.o) main
