#ifndef _LAB5_ARUCO_HPP_
#define _LAB5_ARUCO_HPP_

#include "Defines.hpp"
#include "CoreHeaders.hpp"
#include "ArucoHeaders.hpp"

#include <vector>
#include <iostream>
#include <ctime>

namespace Aruco {

int createCalibrationBoard(const std::string &aOutPathname, int aMarkersCountX, int aMarkersCountY, float aaMarkerSize,
	float aSeparatorSize, int aMargin, int aDictId)
{
	cv::Size image_size(600, 500);

	auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(aDictId));

	auto board = cv::aruco::GridBoard::create(aMarkersCountX, aMarkersCountY, aaMarkerSize, aSeparatorSize, dictionary);

	cv::Mat boardImage;
	board->draw(image_size, boardImage, aMargin);
	cv::imwrite(aOutPathname, boardImage);

	std::cout << "Aruco calibration board was written to " << aOutPathname << std::endl;

	return 0;
}

void saveCameraCalibration(const std::string &aPathname, cv::Size aImageSize, int aSamplesCount,
	const cv::Mat &aCameraMatrix, const cv::Mat &aDistortionCoefficients, double aTotalReprojectionError)
{
	cv::FileStorage fs(aPathname, cv::FileStorage::WRITE);
	if (!fs.isOpened()) {
		std::cout << "Error while saving calibration parameters to" << aPathname << std::endl;
		return;
	}

	std::time_t t;
	std::time(&t);
	std::string timeString = std::ctime(&t);

	fs << "dateTime" << timeString.substr(0, timeString.size() - 1);
	fs << "imageWidth" << aImageSize.width;
	fs << "imageHeight" << aImageSize.height;
	fs << "samplesCount" << aSamplesCount;
	fs << "cameraMatrix" << aCameraMatrix;
	fs << "distortionCoefficients" << aDistortionCoefficients;
	fs << "totalReprojectionError" << aTotalReprojectionError;

	std::cout << "Calibration parameters saved to " << aPathname << std::endl;
}

int calibrateCamera(cv::String aOutputPathname, int aMarkersCountX, int aMarkersCountY, float aaMarkerSize,
	float aSeparatorSize, int aDictId)
{
	using Corners		= std::vector<std::vector<cv::Point2f>>;

	auto detectorParams = cv::aruco::DetectorParameters::create();

	cv::VideoCapture cam;
	cam.open(0);

	int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
	cam.set(cv::CAP_PROP_FOURCC, fourcc);

	auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(aDictId));

	auto gridboard
		= cv::aruco::GridBoard::create(aMarkersCountX, aMarkersCountY, aaMarkerSize, aSeparatorSize, dictionary);

	auto board = gridboard.staticCast<cv::aruco::Board>();

	std::vector<Corners> allCorners;
	std::vector<std::vector<int>> allIds;
	cv::Size imgSize;
	int samplesCount = 0;
	cv::Mat image;

	while (cam.read(image)) {
		cv::Mat copy;

		std::vector<int> ids;
		Corners corners;
		Corners rejectedCorners;

		cv::aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejectedCorners);

		cv::aruco::refineDetectedMarkers(image, board, corners, ids, rejectedCorners);

		image.copyTo(copy);
		if (ids.size() > 0)
			cv::aruco::drawDetectedMarkers(copy, corners, ids);

		cv::imshow("Calibration", copy);

		char key = cv::waitKey(10);
		if (key == Defines::kQuitKey) {
			break;
		}

		if (key == Defines::kSpaceKey && ids.size() > 0) {
			std::cout << "Calibration frame # " << ++samplesCount << std::endl;
			allCorners.push_back(corners);
			allIds.push_back(ids);
			imgSize = image.size();
		}
	}
	cv::destroyAllWindows();

	cv::Mat cameraMatrix		  = cv::Mat::eye(3, 3, CV_64F);
	float aspect_ratio			  = 1;
	cameraMatrix.at<double>(0, 0) = aspect_ratio;
	cv::Mat distCoeffs;
	std::vector<cv::Mat> rvecs, tvecs;
	double reprojectionError;

	Corners cornersConcat;
	std::vector<int> idsConcat;
	std::vector<int> idsCountPerFrame;

	for (const auto &cornerSeq : allCorners) {
		cornersConcat.insert(
			cornersConcat.end(), std::make_move_iterator(cornerSeq.begin()), std::make_move_iterator(cornerSeq.end()));
	}

	for (const auto &idSeq : allIds) {
		idsConcat.insert(idsConcat.end(), std::make_move_iterator(idSeq.begin()), std::make_move_iterator(idSeq.end()));
		idsCountPerFrame.push_back(idSeq.size());
	}

	reprojectionError = cv::aruco::calibrateCameraAruco(
		cornersConcat, idsConcat, idsCountPerFrame, board, imgSize, cameraMatrix, distCoeffs, rvecs, tvecs);

	saveCameraCalibration(aOutputPathname, imgSize, samplesCount, cameraMatrix, distCoeffs, reprojectionError);
	return 0;
}

int createMarker(cv::String aOutputPathname, int aDictId, int aMarkerId, int aMarkerSize, int aBorderBits)
{
	auto dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(aDictId));

	cv::Mat markerImage;
	cv::aruco::drawMarker(dictionary, aMarkerId, aMarkerSize, markerImage, aBorderBits);

	cv::imwrite(aOutputPathname, markerImage);

	return 0;
}

void drawCube(cv::Mat &image, std::vector<cv::Point2f> projectedPoints)
{
	if (image.empty() || projectedPoints.size() != 8)
		return;

	// clang-format off
	std::vector<std::vector<cv::Point2i>> contours = 
	{
		{(cv::Point2i)projectedPoints[0], (cv::Point2i)projectedPoints[1], (cv::Point2i)projectedPoints[2], (cv::Point2i)projectedPoints[3]},
		{(cv::Point2i)projectedPoints[4], (cv::Point2i)projectedPoints[5], (cv::Point2i)projectedPoints[6], (cv::Point2i)projectedPoints[7]},
		{(cv::Point2i)projectedPoints[0], (cv::Point2i)projectedPoints[4], (cv::Point2i)projectedPoints[7], (cv::Point2i)projectedPoints[3]},
		{(cv::Point2i)projectedPoints[0], (cv::Point2i)projectedPoints[4], (cv::Point2i)projectedPoints[5], (cv::Point2i)projectedPoints[1]},
		{(cv::Point2i)projectedPoints[1], (cv::Point2i)projectedPoints[5], (cv::Point2i)projectedPoints[6], (cv::Point2i)projectedPoints[2]},
		{(cv::Point2i)projectedPoints[2], (cv::Point2i)projectedPoints[6], (cv::Point2i)projectedPoints[7], (cv::Point2i)projectedPoints[3]}
	};
	// clang-format on

	cv::drawContours(image, contours, 0, cv::Scalar(255, 0, 0), 1);
	cv::drawContours(image, contours, 1, cv::Scalar(0, 255, 0), 1);
	cv::drawContours(image, contours, 2, cv::Scalar(0, 0, 255), 1);
	cv::drawContours(image, contours, 3, cv::Scalar(0, 0, 255), 1);
	cv::drawContours(image, contours, 4, cv::Scalar(0, 255, 0), 1);
	cv::drawContours(image, contours, 5, cv::Scalar(255, 0, 0), 1);
}

void detectMarkers(cv::String aCalibPathname, float aMarkerSize, int aDictId)
{
	using Corners		 = std::vector<std::vector<cv::Point2f>>;

	cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	cv::Mat distCoeffs;
	cv::FileStorage fs(aCalibPathname, cv::FileStorage::READ);
	if (!fs.isOpened()) {
		std::cout << "Error opening calibration parameters!" << std::endl;
	}

	fs["cameraMatrix"] >> cameraMatrix;
	fs["distortionCoefficients"] >> distCoeffs;

	auto dictionary		= cv::aruco::getPredefinedDictionary(cv::aruco::PREDEFINED_DICTIONARY_NAME(aDictId));

	auto detectorParams = cv::aruco::DetectorParameters::create();

	cv::VideoCapture cam;
	cam.open(0);
	int fourcc = cv::VideoWriter::fourcc('M', 'J', 'P', 'G');
	cam.set(cv::CAP_PROP_FOURCC, fourcc);

	while (cam.grab()) {
		cv::Mat image;
		cv::Mat copy;
		cam.retrieve(image);
		image.copyTo(copy);

		std::vector<int> ids;
		Corners corners;
		Corners rejected;

		cv::aruco::detectMarkers(image, dictionary, corners, ids, detectorParams, rejected);

		if (corners.size() > 0) {
			std::vector<cv::Vec3d> rvecs, tvecs;
			cv::aruco::estimatePoseSingleMarkers(corners, aMarkerSize, cameraMatrix, distCoeffs, rvecs, tvecs);

			float halfSize = aMarkerSize / 2;

			// clang-format off
			std::vector<cv::Point3f> spacialPoints = 
			{
				{-halfSize, -halfSize, 0}, 
				{-halfSize, halfSize, 0},
				{halfSize, halfSize, 0}, 
				{halfSize, -halfSize, 0},
				{-halfSize, -halfSize, halfSize},
				{-halfSize, halfSize, halfSize},
				{halfSize, halfSize, halfSize}, 
				{halfSize, -halfSize, halfSize}
			};
			// clang-format on

			for (size_t i = 0; i < corners.size(); i++) {
				std::vector<cv::Point2f> projectedPoints;
				cv::projectPoints(spacialPoints, rvecs[i], tvecs[i], cameraMatrix, distCoeffs, projectedPoints);

				drawCube(copy, projectedPoints);
			}
		}

		cv::imshow("Detector", copy);
		if (cv::waitKey(1) == Defines::kQuitKey) {
			break;
		}
	}
}

} // namespace Aruco

#endif /* _LAB5_ARUCO_HPP_ */
