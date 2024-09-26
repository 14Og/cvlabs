cmake_minimum_required(VERSION 3.5.0)

set(NAME labs)
project(${NAME} CXX)
include(FindOpenCV.cmake)

set(FLAGS
    "-Wpedantic"
    "-Wall"    
    "-Wextra"
    "-g"
)
string(REPLACE ";" " " FLAGS "${FLAGS}")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS ${FLAGS})

add_subdirectory(lab1)

add_executable(labs main.cpp)
target_link_libraries(${NAME} ${OPENCV_LIBS})

