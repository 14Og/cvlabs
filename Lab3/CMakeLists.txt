
set(NAME Lab3)

include(../cmake/findOpenCV.cmake)

file(GLOB_RECURSE SOURCE_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)


add_executable(${NAME} Main.cpp ${SOURCE_LIST})

target_link_libraries(${NAME} PRIVATE ${OPENCV_LIBS})

