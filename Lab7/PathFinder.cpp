#ifndef _LAB7_PATHFINDER_CPP_
#define _LAB7_PATHFINDER_CPP_

#include "ThinningAlgo.hpp"
#include "Defines.hpp"

#include <vector>
#include <algorithm>

void sortPath(std::vector<cv::Point2i> &aPoints)
{
	if (aPoints.size() <= 1)
		return; // Nothing to sort if empty or single point

	std::vector<bool> visited(aPoints.size(), false);
	visited[0] = true; // Start with the first point as fixed

	// For each position after the first, find the nearest unvisited point
	for (size_t i = 1; i < aPoints.size(); ++i) {
		cv::Point2i current = aPoints[i - 1]; // Last placed point
		double minDist		= std::numeric_limits<double>::max();
		size_t nextIdx		= i;

		// Find the closest unvisited point
		for (size_t j = i; j < aPoints.size(); ++j) {
			if (!visited[j]) {
				double dist = cv::norm(current - aPoints[j]); // Euclidean distance
				if (dist < minDist) {
					minDist = dist;
					nextIdx = j;
				}
			}
		}

		// Swap the closest point into the current position
		if (nextIdx != i) {
			std::swap(aPoints[i], aPoints[nextIdx]);
		}
		visited[i] = true; // Mark the current position as visited
	}
}

void findPathPhoto(cv::Mat &aImage)
{
	cv::Mat thresh;
	cv::cvtColor(aImage, thresh, cv::COLOR_BGR2GRAY);
	cv::threshold(thresh, thresh, 90, 255, cv::THRESH_BINARY);

	auto morphKernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
	cv::dilate(thresh, thresh, morphKernel);
	cv::erode(thresh, thresh, morphKernel);

	Thinning::performFull(thresh);

	std::vector<cv::Vec4i> lines;
	std::vector<cv::Point2i> points;
	cv::HoughLinesP(thresh, lines, 1, CV_PI / 180, 50, 10, 100);

	for (const auto &line : lines) {
		points.emplace_back(line[0], line[1]);
		points.emplace_back(line[2], line[3]);
	}

	auto firstPointIt = std::max_element(points.begin(), points.end(),
		[](const cv::Point2i &aLhs, const cv::Point2i &aRhs) { return cv::norm(aLhs) < cv::norm(aRhs); });

	std::iter_swap(firstPointIt, points.begin());

	sortPath(points);

	for (const auto &point : points) {
		cv::circle(aImage, point, 5, {0, 0, 255}, -1, cv::LINE_8);
	}

	cv::polylines(aImage, points, false, {0, 0, 255}, 1, cv::LINE_AA);

	cv::imshow("thinned path", thresh);
	cv::imshow("path points", aImage);
}

void findPathVideo(cv::VideoCapture &aCap)
{
	cv::Mat image;
	while (aCap.read(image)) {
		findPathPhoto(image);
		if (cv::waitKey(10) == Defines::kQuitKey)
			break;
	}
	aCap.release();
}

int main()
{
	auto photoPath = Defines::resourcesPath / "Lab7" / "Photo" / "Thinning";
	auto videoPath = Defines::resourcesPath / "Lab7" / "Video";

	auto image	   = cv::imread(photoPath / "3.jpg", cv::IMREAD_ANYCOLOR);
	findPathPhoto(image);
	cv::waitKey();

	auto cap = cv::VideoCapture(videoPath / "2.avi");
	findPathVideo(cap);

	cv::destroyAllWindows();
}

#endif /* _LAB7_PATHFINDER_CPP_ */
