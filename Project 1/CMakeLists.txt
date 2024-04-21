cmake_minimum_required(VERSION 3.25)
project(GRAPHICS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(GLUT_INCLUDE_DIR "C:/MinGW/include")
set(GLUT_glut_LIBRARY_RELEASE "C:/MinGW/bin/glut32.dll")
# GLUT
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIR})
if(NOT GLUT_FOUND)
    message(ERROR "GLUT not found!")
endif(NOT GLUT_FOUND)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(NOT OPENGL_FOUND)
    message(ERROR "OPENGL not found!")
endif(NOT OPENGL_FOUND)

add_executable(output project1.cpp )
target_include_directories(output PUBLIC inc)
target_link_libraries(output PRIVATE ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})