
set(NAME lab1)

include(../cmake/findOpenCV.cmake)

file(GLOB_RECURSE SOURCE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
message(STATUS "Sources: " ${SOURCE_LIST})

add_executable(${NAME} Main.cpp ${SOURCE_LIST})

target_link_libraries(${NAME} PRIVATE ${OPENCV_LIBS})

