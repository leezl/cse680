cmake_minimum_required(VERSION 2.6)
project(PA05)

SET(EXECUTABLE_OUTPUT_PATH "${PA05_BINARY_DIR}/bin"
    CACHE INTERNAL "")

#add missing Module finders
set(PA05_MODULE_PATH ${PA05_MODULE_PATH} "${PA05_SOURCE_DIR}/cmake/Modules/")

#find libraries
find_package(Assimp REQUIRED)
find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)

#set flags
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")

#add other directories
add_subdirectory(src)

#find_package(LibXML++ REQUIRED)
#include_directories(${LibXML++_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${LibXML++_LIBRARIES})