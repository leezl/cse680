
CC=g++
CXXFLAGS= -g -Wall -std=c++0x
CXXO= -c 
LIBS= -lglut -lGLEW -lGL
SOURCES= ../src/shader.cpp ../src/main.cpp
DEPS = ../src/shader.h
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=../bin/PA
RM= /bin/rm -f

all: $(EXECUTABLE)

../bin/PA03: shader.o main.o
	$(CC) $(CXXFLAGS) shader.o main.o -o $(EXECUTABLE) $(LIBS)

main.o: ../src/main.cpp ../src/shader.h
	$(CC) $(CXXO) $(CXXFLAGS) ../src/main.cpp -o main.o

shader.o: ../src/shader.cpp ../src/shader.h
	$(CC) $(CXXO) $(CXXFLAGS) ../src/shader.cpp -o shader.o

clean:
	$(RM) *.o $(EXECUTABLE)
