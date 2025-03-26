
set(NAME Lab6)

include(../cmake/findOpenCV.cmake)

add_executable(Photo Photo.cpp PhotoLocator.hpp)
add_executable(Video Video.cpp VideoLocator.hpp)


target_link_libraries(Photo PRIVATE ${OPENCV_LIBS})
target_link_libraries(Video PRIVATE ${OPENCV_LIBS})
