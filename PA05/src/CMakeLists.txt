#src CMakeLists

include_directories(${PA05_SOURCE_DIR}/src)

add_executable(PA main.cpp shader.cpp object.cpp)

#only object needs Assimp
set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY} ${ASSIMP_LIBRARIES})

target_link_libraries(PA CORELIBS)