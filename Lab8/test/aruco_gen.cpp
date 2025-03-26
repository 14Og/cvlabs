#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>

#include "utils.hpp"

void gen_marker()
{
    cv::Mat markerImage;
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::generateImageMarker(dictionary, 23, 200, markerImage, 1);
    cv::imshow("marker", markerImage);

    while (cv::waitKey() != (int)'q')
        {
        }
}



int main(void)
{
    create_board(std::string(SOURCE_DIR) + "board.png", 5, 7, 80, 10, 0, 6, true);
}
