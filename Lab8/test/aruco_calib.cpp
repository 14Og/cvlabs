#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>


int main(void)
{
    calibrate_cam(std::string(SOURCE_DIR) + "cam.yml", 5, 7, 80, 10, 6, std::string(SOURCE_DIR) + "detector_params.yml");
}
