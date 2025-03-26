#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>


int main(void)
{
    detect_board(5, 7, 80, 10, 6, false, 0, std::string(SOURCE_DIR) + "cam.yml", std::string(SOURCE_DIR) + "detector_params.yml");
}
