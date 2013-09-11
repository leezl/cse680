
CC=g++
CXXFLAGS= -g -Wall -std=c++0x
LIBS= -lglut -lGLEW -lGL
SOURCES= ../src/shader.cpp ../src/main.cpp
DEPS = ../src/shader.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=PA02

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)

.cpp.o: $(DEPS)
	$(CC) $(CXXFLAGS) $< -o $@ $(LIBS)

clean:
	rm -f obj/*.o *~ core *~ 
