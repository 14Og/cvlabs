option(BUILD_SHARED_LIBS "Build simple_socket shared library" OFF)
ADD_DEFINITIONS(-DSOURCE_DIR="${PROJECT_SOURCE_DIR}/")

add_library(aruco_lib)
target_link_libraries(
    aruco_lib
  PUBLIC
    ${OpenCV_LIBS})
target_include_directories(
    aruco_lib
  PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)
target_sources(
    aruco_lib
  PRIVATE
    detect.cpp
    create.cpp
    calibrate.cpp
)
