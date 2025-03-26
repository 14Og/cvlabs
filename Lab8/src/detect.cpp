#include "utils.hpp"

using namespace std;
using namespace cv;


/**
 * @brief
 *
 * @param markersX Number of squares in X direction
 * @param markersY Number of squares in Y direction
 * @param markerLength Marker side length (in pixels)
 * @param markerSeparation Separation between two consecutive markers in the grid (in pixels)}
 * @param dictionaryId dictionary: DICT_4X4_50=0, DICT_4X4_100=1, DICT_4X4_250=2,"
    "DICT_4X4_1000=3, DICT_5X5_50=4, DICT_5X5_100=5, DICT_5X5_250=6, DICT_5X5_1000=7, "
    "DICT_6X6_50=8, DICT_6X6_100=9, DICT_6X6_250=10, DICT_6X6_1000=11, DICT_7X7_50=12,"
    "DICT_7X7_100=13, DICT_7X7_250=14, DICT_7X7_1000=15, DICT_ARUCO_ORIGINAL = 16
 * @param refindStrategy Apply refind strategy
 * @param camId
 * @param camparam
 * @param detectorparam
 */
void detect_board(int markersX,
                  int markersY,
                  int markerLength,
                  int markerSeparation,
                  int dictionaryId,
                  bool refindStrategy,
                  int camId = 0,
                  std::string camparam = "",
                  std::string detectorparam = "")
{
    bool showRejected = false;
    auto dictionary = cv::aruco::getPredefinedDictionary(dictionaryId);


    Mat camMatrix, distCoeffs;

    if (!camparam.empty())
        if (!readCameraParameters(camparam, camMatrix, distCoeffs))
            throw std::runtime_error("Invalid camera file\n");

    cv::aruco::DetectorParameters detectorParams;
    if (!detectorparam.empty())
    {
        cv::FileStorage fs(detectorparam, cv::FileStorage::READ);
        bool readOk = detectorParams.readDetectorParameters(fs.root());
        if (!readOk)
            throw std::runtime_error("Invalid detector parameters file\n");
    }


    aruco::ArucoDetector detector(dictionary, detectorParams);
    VideoCapture inputVideo;
    inputVideo.open(camId);
    int waitTime = 10;


    float axisLength =
        0.5f *
        ((float)min(markersX, markersY) * (markerLength + markerSeparation) + markerSeparation);

    // Create GridBoard object
    //! [aruco_create_board]
    aruco::GridBoard board(Size(markersX, markersY), markerLength, markerSeparation, dictionary);
    //! [aruco_create_board]

    // Also you could create Board object
    // vector<vector<Point3f> > objPoints; // array of object points of all the marker corners in
    // the board vector<int> ids; // vector of the identifiers of the markers in the board
    // aruco::Board board(objPoints, dictionary, ids);

    double totalTime = 0;
    int totalIterations = 0;

    while (inputVideo.grab())
    {
        Mat image, imageCopy;
        inputVideo.retrieve(image);

        double tick = (double)getTickCount();

        vector<int> ids;
        vector<vector<Point2f>> corners, rejected;
        Vec3d rvec, tvec;

        //! [aruco_detect_and_refine]

        // Detect markers
        detector.detectMarkers(image, corners, ids, rejected);

        // Refind strategy to detect more markers
        if (refindStrategy)
            detector
                .refineDetectedMarkers(image, board, corners, ids, rejected, camMatrix, distCoeffs);

        //! [aruco_detect_and_refine]

        // Estimate board pose
        int markersOfBoardDetected = 0;
        if (!ids.empty())
        {
            // Get object and image points for the solvePnP function
            cv::Mat objPoints, imgPoints;
            board.matchImagePoints(corners, ids, objPoints, imgPoints);

            // Find pose
            cv::solvePnP(objPoints, imgPoints, camMatrix, distCoeffs, rvec, tvec);

            markersOfBoardDetected = (int)objPoints.total() / 4;
        }

        double currentTime = ((double)getTickCount() - tick) / getTickFrequency();
        totalTime += currentTime;
        totalIterations++;
        if (totalIterations % 30 == 0)
        {
            cout << "Detection Time = " << currentTime * 1000 << " ms "
                 << "(Mean = " << 1000 * totalTime / double(totalIterations) << " ms)" << endl;
        }

        // Draw results
        image.copyTo(imageCopy);
        if (!ids.empty())
            aruco::drawDetectedMarkers(imageCopy, corners, ids);

        if (showRejected && !rejected.empty())
            aruco::drawDetectedMarkers(imageCopy, rejected, noArray(), Scalar(100, 0, 255));

        if (markersOfBoardDetected > 0)
            cv::drawFrameAxes(imageCopy, camMatrix, distCoeffs, rvec, tvec, axisLength);

        imshow("out", imageCopy);
        char key = (char)waitKey(waitTime);
        if (key == 27)
            break;
        //! [aruco_detect_board_full_sample]
    }
}