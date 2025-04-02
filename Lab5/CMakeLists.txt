set(NAME Lab5)

include(../cmake/findOpenCV.cmake)
include(../cmake/findAruco.cmake)

# add_executable(Thinning ThinningAlgo.hpp Thinning.cpp)
# add_executable(PathFinder ThinningAlgo.hpp PathFinder.cpp)
# add_executable(Currency ThinningAlgo.hpp Currency.cpp)

add_executable(Aruco Aruco.hpp Main.cpp)
add_executable(MakePhoto MakePhoto.cpp )

target_link_libraries(Aruco PRIVATE ${OPENCV_LIBS} ${ARUCO_LIBS})
target_link_libraries(MakePhoto PRIVATE ${OPENCV_LIBS})