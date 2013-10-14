cmake_minimum_required(VERSION 2.6)
project(PA)

cmake_policy(SET CMP0017 NEW)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PA_SOURCE_DIR}/bin")

#message("binary dir " ${PA_BINARY_DIR})

#add missing Module finders
#This wasn't set on one of my computers...
SET(CMAKE_MODULE_PATH "${CMAKE_ROOT}/Modules")
#on one the first path worked, on another the second...
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PA_SOURCE_DIR}/cmake/Modules/")
set(PA_MODULE_PATH ${PA_MODULE_PATH} "${PA_SOURCE_DIR}/cmake/Modules/")

#find libraries
find_package(Assimp REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(DevIL REQUIRED)

include_directories(${IL_INCLUDE_DIR})
message("IL directories " ${IL_INCLUDE_DIR})
include_directories(${PA_SOURCE_DIR}/src)

#only object needs Assimp
set(CORELIBS 
	${GLUT_LIBRARY} 
	${GLEW_LIBRARY} 
	${OPENGL_LIBRARY} 
	${ASSIMP_LIBRARIES} 
	${IL_LIBRARIES} 
	${ILU_LIBRARIES}
	#${ILUT_LIBRARIES}
	)

#set flags
set(CMAKE_CXX_FLAGS "-g -Wall -std=c++0x")

#find_package(LibXML++ REQUIRED)
#include_directories(${LibXML++_INCLUDE_DIRS})
#set(LIBS ${LIBS} ${LibXML++_LIBRARIES})

set(SOURCES 
	${PA_SOURCE_DIR}/src/main.cpp 
	${PA_SOURCE_DIR}/src/shader.cpp 
	${PA_SOURCE_DIR}/src/object.cpp
	${PA_SOURCE_DIR}/src/program.cpp
	${PA_SOURCE_DIR}/src/texture.cpp)

add_executable(pa7 ${SOURCES})

target_link_libraries(pa7 ${CORELIBS})
