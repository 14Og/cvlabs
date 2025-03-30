#ifndef _LAB6_IMAGELOCATOR_HPP_
#define _LAB6_IMAGELOCATOR_HPP_

#include "Core.hpp"
#include "Defines.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <array>

class PhotoLocator {

protected:
	using Points1D = std::vector<cv::Point>;
	using Points2D = std::vector<cv::Point2d>;

public:
	PhotoLocator() = default;

	void locate(cv::Mat aImageSource, bool aUseMorph = false)
	{
		cv::Mat res, thresh;

		// rgb -> grayscale
		cv::cvtColor(aImageSource, thresh, cv::COLOR_BGR2GRAY);

		// thresholding laser points
		cv::threshold(thresh, thresh, 160, 255, cv::THRESH_BINARY);

		if (aUseMorph) {
			auto morphKernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
			cv::erode(thresh, thresh, morphKernel);
			cv::dilate(thresh, thresh, morphKernel);
		}

		cv::Mat kernel(2, 1, CV_32SC1, {1, -1});

		// filtering points to enhance vertical diff
		cv::filter2D(thresh, thresh, CV_8UC1, kernel);

		Points1D points		= detectLaserPoints(thresh);

		Points2D transformedPoints = transform(points, 0.006, 0.14, 0.06, aImageSource.rows, aImageSource.cols);

		int mapW			= 800;
		int mapH			= 600;

		cv::Mat map			= cv::Mat::zeros(mapH, mapW, CV_8UC3);
		createGrid(map, 25);

		for (const auto &p : transformedPoints) {
			cv::Point target(static_cast<int>(p.y * 250) + mapW / 2, -static_cast<int>(p.x * 250) + mapH - 10);
			cv::circle(map, target, 3, cv::Scalar(0, 255, 0), -1);
		}

		cv::imshow("source image", aImageSource);
		cv::imshow("thresholded", thresh);
		cv::imshow("transformed map", map);
	}

protected:
	Points2D transform(const Points1D &aSrcPoints, 
						const double aF, 
						const double aCamH, 
						const double aLaserH,
						const int aRows, 
						const int aCols, 
						const double aCx = 0.05, 
						const double aCy = 0.0035)
	{
		Points2D result;

		for (const auto &src_p : aSrcPoints) {
			auto dx = aCx * static_cast<double>(src_p.x - aCols / 2) / static_cast<double>(aCols);
			auto dy = aCy * static_cast<double>(src_p.y - aRows / 2) / static_cast<double>(aRows);
			auto x	= (aCamH - aLaserH) * (aF / dy);
			auto z	= x * dx / aF;
			result.emplace_back(x, z);
		}

		return result;
	}

	Points1D detectLaserPoints(const cv::Mat1b &src)
	{
		Points1D result;

		for (int y = 0; y < src.rows; ++y) {
			for (int x = 0; x < src.cols; ++x)
				if (src.at<uchar>(y, x) == 255)
					result.emplace_back(x, y);
		}

		return result;
	}

	void createGrid(cv::Mat &aImage, int aGridStep)
	{
		auto width  = aImage.cols;
		auto height = aImage.rows;

		cv::line(aImage, cv::Point(width / 2, 0), cv::Point(width / 2, height), cv::Scalar(255, 255, 255), 1);

		for (int y = 0; y < height; y += aGridStep)
			cv::line(aImage, cv::Point(0, y), cv::Point(width, y), cv::Scalar(255, 255, 255), 1);
	}
};

#endif /* _LAB6_IMAGELOCATOR_HPP_ */
