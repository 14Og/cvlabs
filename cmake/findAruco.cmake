find_package(OpenCV REQUIRED calib3d aruco)

if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    include_directories((${CMAKE_CURRENT_SOURCE_DIR}))
    set(ARUCO_LIBS ${OpenCV_LIBS} CACHE INTERNAL "OpenCV aruco libraries")
endif()
