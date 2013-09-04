# Linux
CC=g++

# For Macs uncomment the next line and comment out the previous one
#CC=clang++
# Assuming you want to use a recent compiler

# Compiler flags
LIBS= -lglut -lGLEW -lGL
CXXFLAGS= -g -Wall -std=c++0x

all: ../bin/PA02

../bin/PA02: ../src/main.cpp
	$(CC) $(CXXFLAGS) ../src/main.cpp -o ../bin/PA02 $(LIBS)

