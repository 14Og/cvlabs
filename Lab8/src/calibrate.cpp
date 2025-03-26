#include "aruco_samples_utility.hpp"

#include <ctime>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <vector>

using namespace std;
using namespace cv;


namespace {
const char* about =
    "Calibration using a ArUco Planar Grid board\n"
    "  To capture a frame for calibration, press 'c',\n"
    "  If input comes from video, press any key for next frame\n"
    "  To finish capturing, press 'ESC' key and calibration starts.\n";
const char* keys =
    "{w        |       | Number of squares in X direction }"
    "{h        |       | Number of squares in Y direction }"
    "{l        |       | Marker side length (in meters) }"
    "{s        |       | Separation between two consecutive markers in the grid (in meters) }"
    "{d        |       | dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
    "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
    "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
    "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16}"
    "{cd       |       | Input file with custom dictionary }"
    "{@outfile |cam.yml| Output file with calibrated camera parameters }"
    "{v        |       | Input from video file, if ommited, input comes from camera }"
    "{ci       | 0     | Camera id if input doesnt come from video (-v) }"
    "{dp       |       | File of marker detector parameters }"
    "{rs       | false | Apply refind strategy }"
    "{zt       | false | Assume zero tangential distortion }"
    "{a        |       | Fix aspect ratio (fx/fy) to this value }"
    "{pc       | false | Fix the principal point at the center }";
}  // namespace


void calibrate_cam(std::string outputFile,
                  int markersX,
                  int markersY,
                  int markerLength,
                  int markerSeparation,
                  int dictionaryId,
                  std::string detectorparam,
                  bool refindStrategy,
                  bool calib_zero_tangent,
                  bool calib_fix_principal,
                  float aspectRatio,
                  int camId)
{
    int calibrationFlags = 0;
    if (aspectRatio != 1)
        calibrationFlags |= CALIB_FIX_ASPECT_RATIO;

    if (calib_zero_tangent)
        calibrationFlags |= CALIB_ZERO_TANGENT_DIST;
    if (calib_fix_principal)
        calibrationFlags |= CALIB_FIX_PRINCIPAL_POINT;

    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);
    cv::aruco::DetectorParameters detectorParams;
    if (!detectorparam.empty())
    {
        cv::FileStorage fs(detectorparam, cv::FileStorage::READ);
        bool readOk = detectorParams.readDetectorParameters(fs.root());
        if (!readOk)
            throw std::runtime_error("Invalid detector parameters file\n");
    }

    String video;

    VideoCapture inputVideo;
    inputVideo.open(camId);
    int waitTime = 10;

    //! [CalibrationWithArucoBoard1]
    // Create board object and ArucoDetector
    aruco::GridBoard gridboard(Size(markersX, markersY),
                               markerLength,
                               markerSeparation,
                               dictionary);
    aruco::ArucoDetector detector(dictionary, detectorParams);

    // Collected frames for calibration
    vector<vector<vector<Point2f>>> allMarkerCorners;
    vector<vector<int>> allMarkerIds;
    Size imageSize;

    while (inputVideo.grab())
    {
        Mat image, imageCopy;
        inputVideo.retrieve(image);

        vector<int> markerIds;
        vector<vector<Point2f>> markerCorners, rejectedMarkers;

        // Detect markers
        detector.detectMarkers(image, markerCorners, markerIds, rejectedMarkers);

        // Refind strategy to detect more markers
        if (refindStrategy)
        {
            detector
                .refineDetectedMarkers(image, gridboard, markerCorners, markerIds, rejectedMarkers);
        }
        //! [CalibrationWithArucoBoard1]

        // Draw results
        image.copyTo(imageCopy);

        if (!markerIds.empty())
            aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);

        putText(imageCopy,
                "Press 'c' to add current frame. 'ESC' to finish and calibrate",
                Point(10, 20),
                FONT_HERSHEY_SIMPLEX,
                0.5,
                Scalar(255, 0, 0),
                2);
        imshow("out", imageCopy);

        // Wait for key pressed
        char key = (char)waitKey(waitTime);

        if (key == 27)
            break;

        //! [CalibrationWithArucoBoard2]
        if (key == 'c' && !markerIds.empty())
        {
            cout << "Frame captured" << endl;
            allMarkerCorners.push_back(markerCorners);
            allMarkerIds.push_back(markerIds);
            imageSize = image.size();
        }
    }
    //! [CalibrationWithArucoBoard2]

    if (allMarkerIds.empty())
        throw std::runtime_error("Not enough captures for calibration\n");

    //! [CalibrationWithArucoBoard3]
    Mat cameraMatrix, distCoeffs;

    if (calibrationFlags & CALIB_FIX_ASPECT_RATIO)
    {
        cameraMatrix = Mat::eye(3, 3, CV_64F);
        cameraMatrix.at<double>(0, 0) = aspectRatio;
    }

    // Prepare data for calibration
    vector<Point3f> objectPoints;
    vector<Point2f> imagePoints;
    vector<Mat> processedObjectPoints, processedImagePoints;
    size_t nFrames = allMarkerCorners.size();

    for (size_t frame = 0; frame < nFrames; frame++)
    {
        Mat currentImgPoints, currentObjPoints;

        gridboard.matchImagePoints(allMarkerCorners[frame],
                                   allMarkerIds[frame],
                                   currentObjPoints,
                                   currentImgPoints);

        if (currentImgPoints.total() > 0 && currentObjPoints.total() > 0)
        {
            processedImagePoints.push_back(currentImgPoints);
            processedObjectPoints.push_back(currentObjPoints);
        }
    }

    // Calibrate camera
    double repError = calibrateCamera(
        processedObjectPoints,
        processedImagePoints,
        imageSize,
        cameraMatrix,
        distCoeffs,
        noArray(),
        noArray(),
        noArray(),
        noArray(),
        noArray(),
        calibrationFlags);
    //! [CalibrationWithArucoBoard3]
    bool saveOk = saveCameraParams(
        outputFile,
        imageSize,
        aspectRatio,
        calibrationFlags,
        cameraMatrix,
        distCoeffs,
        repError);

    if (!saveOk)
        throw std::runtime_error("Cannot save output file\n");

    cout << "Rep Error: " << repError << endl;
    cout << "Calibration saved to " << outputFile << endl;
}