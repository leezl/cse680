
CC=g++
CXXFLAGS= -g -Wall -std=c++0x
CXXO= -c 
LIBS= -lglut -lGLEW -lGL -lGLU
SOURCES= ../src/shader.cpp ../src/main.cpp
DEPS = ../src/shader.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../bin/PA
RM= /bin/rm -f

all: $(EXECUTABLE)

$(EXECUTABLE): shader.o object.o main.o
	$(CC) $(CXXFLAGS) object.o shader.o main.o -o $(EXECUTABLE) $(LIBS)

main.o: ../src/main.cpp ../src/shader.h
	$(CC) $(CXXO) $(CXXFLAGS) ../src/main.cpp -o main.o

shader.o: ../src/shader.cpp ../src/shader.h
	$(CC) $(CXXO) $(CXXFLAGS) ../src/shader.cpp -o shader.o

object.o: ../src/object.cpp ../src/object.h
	$(CC) $(CXXO) $(CXXFLAGS) ../src/object.cpp -o object.o

clean:
	$(RM) *.o $(EXECUTABLE)
