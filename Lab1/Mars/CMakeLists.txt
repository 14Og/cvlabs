cmake_minimum_required(VERSION 3.20)
project(three_body_problem)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
find_package(OpenCV REQUIRED)
add_executable(${PROJECT_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${OpenCV_INCLUDE_DIRS}, 
    ${CMAKE_CURRENT_SOURCE_DIR}/)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})