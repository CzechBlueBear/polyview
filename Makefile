CXXFLAGS=-Wall -Og -ggdb --std=c++23
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf

HEADERS=sdl_wrapper.hpp document.hpp

%.o: %.cpp ${HEADERS} Makefile
	c++ ${CXXFLAGS} -c $*.cpp -o $*.o

app: sdl_wrapper.o document.o main.o
	c++ $^ -o $@ ${LIBS}
