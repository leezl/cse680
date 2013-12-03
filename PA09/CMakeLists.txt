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
#SET( BULLET_ROOT "/home/liesl/Projects/bullet-2.81-rev2613")
#set (BULLET_BUILD_DIR "/home/liesl/Projects/bullet-2.81-rev2613/bulletBuild")
#set (BULLET_SOURCE_DIR "/usr/include/bullet")
set (BULLET_SOURCE_DIR "/usr/local/include/bullet")

#find libraries
find_package(Assimp REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(DevIL REQUIRED)
#several bullet finders online, none in my cmake...
find_package(BULLET REQUIRED)

include_directories(${IL_INCLUDE_DIR})
include_directories(${BULLET_INCLUDE_DIR})
#message("bullets " ${BULLET_INCLUDE_DIR})
include_directories(${PA_SOURCE_DIR}/src)

#message("Libs " ${BULLET_LIBRARIES})

#only object needs Assimp
set(CORELIBS 
	${GLUT_LIBRARY} 
	${GLEW_LIBRARY} 
	${OPENGL_LIBRARY} 
	${ASSIMP_LIBRARIES} 
	${IL_LIBRARIES} 
	${ILU_LIBRARIES}
	#${ILUT_LIBRARIES}
	${BULLET_BulletDynamics_LIBRARY}
	${BULLET_BulletCollision_LIBRARY}
	${BULLET_LinearMath_LIBRARY}
	#${BULLET_LIBRARIES}
	#${BULLET_SoftBody_LIBRARY}
	#${BULLET_BulletDynamics_LIBRARY}
	#${BULLET_BulletCollision_LIBRARY}
	#${BULLET_LinearMath_LIBRARY}
	)
	#	#${ILUT_LIBRARIES}
	#${BULLET_LIBRARIES}

#message("oh ", ${CORELIBS})

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
	${PA_SOURCE_DIR}/src/texture.cpp
	${PA_SOURCE_DIR}/src/physics.cpp
	${PA_SOURCE_DIR}/src/objectBillboard.cpp
	${PA_SOURCE_DIR}/src/AI.cpp
	${PA_SOURCE_DIR}/src/mesh.cpp
	${PA_SOURCE_DIR}/src/objectPhysics.cpp
)

add_executable(pa ${SOURCES})

target_link_libraries(pa ${CORELIBS})
