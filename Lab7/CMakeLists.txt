set(NAME Lab7)

include(../cmake/findOpenCV.cmake)

add_executable(Thinning ThinningAlgo.hpp Thinning.cpp)
add_executable(PathFinder ThinningAlgo.hpp PathFinder.cpp)
add_executable(Currency ThinningAlgo.hpp Currency.cpp)


target_link_libraries(Thinning PRIVATE ${OPENCV_LIBS})
target_link_libraries(PathFinder PRIVATE ${OPENCV_LIBS})
target_link_libraries(Currency PRIVATE ${OPENCV_LIBS})
