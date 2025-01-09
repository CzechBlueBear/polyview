CXXFLAGS=-Wall -Og -ggdb --std=c++23
LIBS=-lSDL2 -lSDL2_image -lSDL2_ttf

HEADERS=sdl_wrapper.hpp document.hpp view.hpp settings.hpp widget.hpp

%.o: %.cpp ${HEADERS} Makefile
	c++ ${CXXFLAGS} -c $*.cpp -o $*.o

app: sdl_wrapper.o document.o main.o view.o widget.o
	c++ $^ -o $@ ${LIBS}

clean:
	rm *.o app
