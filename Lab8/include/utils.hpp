#include "aruco_samples_utility.hpp"

#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <vector>

void create_board(cv::String out,
                  int markersX,
                  int markersY,
                  int markerLength,
                  int markerSeparation,
                  int margins,
                  int dictionaryId,
                  bool showImage);

void detect_board(int markersX,
                  int markersY,
                  int markerLength,
                  int markerSeparation,
                  int dictionaryId,
                  bool refindStrategy,
                  int camId,
                  std::string camparam,
                  std::string detectorparam);

void calibrate_cam(
    std::string outputFile,
    int markersX,
    int markersY,
    int markerLength,
    int markerSeparation,
    int dictionaryId,
    std::string detectorparam,
    bool refindStrategy = false,
    bool calib_zero_tangent = false,
    bool calib_fix_principal = false,
    float aspectRatio = 1,
    int camId = 0);