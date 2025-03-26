option(BUILD_SHARED_LIBS "Build simple_socket shared library" OFF)

add_executable(create_board create_board.cpp)
target_link_libraries(create_board
  PUBLIC
    ${OpenCV_LIBS}
)
target_include_directories(
  create_board
  PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)

add_executable(detect_board detect_board.cpp)
target_link_libraries(detect_board
  PUBLIC
    ${OpenCV_LIBS}
)
target_include_directories(
  detect_board
  PUBLIC
    ${OpenCV_INCLUDE_DIRS}
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>"
)