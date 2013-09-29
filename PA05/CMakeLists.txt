cmake_minimum_required(VERSION 2.6)
project(PA05)

cmake_policy(SET CMP0017 NEW)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PA05_SOURCE_DIR}/bin")

#message("binary dir " ${PA05_BINARY_DIR})

#add missing Module finders
#This wasn't set on one of my computers...
SET(CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
#on one the first path worked, on another the second...
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PA05_SOURCE_DIR}/cmake/Modules/")
set(PA05_MODULE_PATH ${PA05_MODULE_PATH} "${PA05_SOURCE_DIR}/cmake/Modules/")

include_directories(${PA05_SOURCE_DIR}/src)

#find libraries
find_package(Assimp REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)

#only object needs Assimp
set(CORELIBS ${GLUT_LIBRARY} ${GLEW_LIBRARY} ${OPENGL_LIBRARY} ${ASSIMP_LIBRARIES})

#set flags
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")

#find_package(LibXML++ REQUIRED)
#include_directories(${LibXML++_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

set(SOURCES ${PA05_SOURCE_DIR}/src/main.cpp ${PA05_SOURCE_DIR}/src/shader.cpp ${PA05_SOURCE_DIR}/src/object.cpp)

add_executable(PA ${SOURCES})

target_link_libraries(PA ${CORELIBS})
