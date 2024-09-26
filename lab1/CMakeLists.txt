
set(NAME lab1)

include(../FindOpenCV.cmake)

file(GLOB_RECURSE SOURCE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)

add_library(${NAME} OBJECT ${SOURCE_LIST})

target_link_libraries(${NAME} PRIVATE ${OPENCV_LIBS})

