find_package(OpenCV REQUIRED calib3d core imgproc highgui imgcodecs)

if(OpenCV_FOUND)
    include_directories(${OpenCV_INCLUDE_DIRS})
    set(OPENCV_LIBS ${OpenCV_LIBS} CACHE INTERNAL "OpenCV libraries")
endif()