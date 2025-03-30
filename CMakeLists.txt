cmake_minimum_required(VERSION 3.5.0)

set(NAME labs)
project(${NAME} CXX)
include(cmake/findOpenCV.cmake)

add_compile_definitions(MEDIA_DIR="${PROJECT_SOURCE_DIR}/Media")

set(FLAGS
    "-Wpedantic"
    "-Wall"    
    "-Wextra"
    "-g"
)

string(REPLACE ";" " " FLAGS "${FLAGS}")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS ${FLAGS})

# add_subdirectory(Lab1)
# add_subdirectory(Lab2)
# add_subdirectory(Lab3)
# add_subdirectory(Lab4)

add_subdirectory(Lab6)
add_subdirectory(Lab7)
