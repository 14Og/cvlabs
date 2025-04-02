#ifndef LAB3_TASK1
#define LAB3_TASK1

#include "CoreHeaders.hpp"
#include "Base.hpp"
#include "Defines.hpp"


#include <string>
#include <filesystem>
#include <ranges>
#include <iostream>

namespace Lab3 {

class WarmPlacesFinder : public FinderBase {

	static constexpr auto kPhotosExt{".jpg"};
	static constexpr auto kVideosExt{".mp4"};
	using Path = std::filesystem::path;
	using Iter = std::filesystem::directory_iterator;

public:
	WarmPlacesFinder(const std::string aMediaPathname): FinderBase(std::move(aMediaPathname))
	{
	}

	void findOnPhotos([[maybe_unused]] DisplayMode aMode) final
	{
		for (const auto &pathname : absPhotoPathnames) {
			auto sourceImage = cv::imread(pathname, cv::IMREAD_COLOR);
			detectWarmPlaces(sourceImage);
			auto windowName = "Warm places: " + pathname.filename().string();
			cv::imshow(windowName, sourceImage);
			if (aMode == DisplayMode::SUBSEQUENTIAL) {
				cv::waitKey();
				cv::destroyWindow(windowName);
			} else
				cv::waitKey(100);
		}
		if (cv::waitKey() == Defines::kQuitKey)
			cv::destroyAllWindows();
	}

	void findOnVideos([[maybe_unused]] DisplayMode aMode) final
	{
		for (const auto &pathname : absVideoPathnames) {
			cv::VideoCapture videoCapture(pathname);
			cv::Mat frame;
			auto windowName = "Warm places: " + pathname.filename().string();
			while (videoCapture.read(frame)) {
				detectWarmPlaces(frame);
				cv::imshow(windowName, frame);
				cv::waitKey(40);
			}
			if (aMode == DisplayMode::SUBSEQUENTIAL) {
				cv::waitKey();
				cv::destroyWindow(windowName);
			} else
				cv::waitKey(100);
		}
		if (cv::waitKey() == Defines::kQuitKey)
			cv::destroyAllWindows();
	}

private:
	void detectWarmPlaces(const cv::Mat &aSourceImage)
	{
		cv::Mat workingImage;
		cv::Mat workImage;
		cv::cvtColor(aSourceImage, workImage, cv::COLOR_BGR2GRAY);

		cv::threshold(workImage, workImage, 220, 255, cv::THRESH_BINARY);
		auto erodeKernel  = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
		auto dilateKernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
		cv::erode(workImage, workImage, erodeKernel, {1, 1}, 2);
		cv::dilate(workImage, workImage, dilateKernel, {1, 1}, 3);

		std::vector<std::vector<cv::Point>> contours;
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		for (auto &contour : contours) {
			auto moment	 = cv::moments(contour);
			auto xCenter = static_cast<int>(moment.m10 / moment.m00);
			auto yCenter = static_cast<int>(moment.m01 / moment.m00);
			auto rect	 = cv::boundingRect(contour);
			cv::circle(aSourceImage, {xCenter, yCenter}, 3, cv::Scalar(0, 0, 255), -1);
			cv::rectangle(aSourceImage, rect, cv::Scalar(0, 255, 0), 1);
			std::string centerInfo = std::to_string(xCenter) + ";" + std::to_string(yCenter);
			cv::putText(aSourceImage, centerInfo, {rect.x - 10, rect.y - 10}, cv::FONT_HERSHEY_PLAIN, 0.7,
				cv::Scalar(0, 255, 0), 1);
		}
	}

}; // namespace Lab3

}
#endif /* LAB3_TASK1 */
