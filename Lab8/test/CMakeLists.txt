ADD_DEFINITIONS(-DSOURCE_DIR="${PROJECT_SOURCE_DIR}/")

add_executable(aruco_gen aruco_gen.cpp)
target_link_libraries(aruco_gen
  PUBLIC
    ${OpenCV_LIBS}
    aruco_lib
)
target_include_directories(
  aruco_gen
  PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)
add_test(NAME ArucoGenTest
COMMAND aruco_gen)


add_executable(aruco_detect aruco_detect.cpp)
target_link_libraries(aruco_detect
  PUBLIC
    ${OpenCV_LIBS}
    aruco_lib
)
target_include_directories(
  aruco_detect
  PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)
add_test(NAME ArucoDetectTest
COMMAND aruco_detect)

add_executable(aruco_calib aruco_calib.cpp)
target_link_libraries(aruco_calib
  PUBLIC
    ${OpenCV_LIBS}
    aruco_lib
)
target_include_directories(
  aruco_calib
  PUBLIC
    ${OpenCV_INCLUDE_DIRS}
)
add_test(NAME ArucoCalibTest
COMMAND aruco_calib)