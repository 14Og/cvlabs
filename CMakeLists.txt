cmake_minimum_required(VERSION 3.5.0)

set(NAME labs)
project(${NAME} CXX)
include(cmake/findOpenCV.cmake)

set(FLAGS
    "-Wpedantic"
    "-Wall"    
    "-Wextra"
    "-g"
)

string(REPLACE ";" " " FLAGS "${FLAGS}")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS ${FLAGS})

add_subdirectory(Lab1)

add_executable(labs Main.cpp)
target_link_libraries(${NAME} ${OPENCV_LIBS})

