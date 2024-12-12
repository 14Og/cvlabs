#ifndef LAB3_TASK2
#define LAB3_TASK2

#include "Core.hpp"
#include "Defines.hpp"
#include "Base.hpp"

#include <iostream>

namespace Lab3 {

class EngineCompartmentFinder : public FinderBase {


public:
	EngineCompartmentFinder(const std::string aMediaPathname):
		FinderBase(std::move(aMediaPathname)), hsvThresholdHelper(absPhotoPathnames)
	{
	}

	void findOnPhotos([[maybe_unused]] DisplayMode aMode) final
	{
		for (const auto &pathname : absPhotoPathnames) {
			auto sourceImage = cv::imread(pathname, cv::IMREAD_COLOR);
			auto kernel		 = cv::getStructuringElement(cv::MORPH_RECT, {kMorphKernelSize, kMorphKernelSize});

			cv::Mat workImage;

			cv::cvtColor(sourceImage, workImage, cv::COLOR_BGR2HSV);
			cv::inRange(workImage, threshMin, threshMax, workImage);
			cv::dilate(workImage, workImage, kernel);
			cv::erode(workImage, workImage, kernel);

			std::vector<std::vector<cv::Point>> contours;
			cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

			for (auto &contour : contours) {
				auto moment	 = cv::moments(contour);
				auto xCenter = static_cast<int>(moment.m10 / moment.m00);
				auto yCenter = static_cast<int>(moment.m01 / moment.m00);
				cv::circle(sourceImage, {xCenter, yCenter}, 3, cv::Scalar(0,0,0), -1);

				auto rect	 = cv::boundingRect(contour);
				cv::putText(sourceImage, "engine", {rect.x - 10, rect.y - 10}, cv::FONT_HERSHEY_COMPLEX, 0.5,
				cv::Scalar(255,255,255), 1);

			}
			auto windowName = "Engine: " + pathname.filename().string();
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

public:
	HSVThresholdHelper hsvThresholdHelper;

private:
	 cv::Scalar threshMin{0, 140, 130};
	 cv::Scalar threshMax{15, 255, 255};
	 int kMorphKernelSize{25};


};

} // namespace Lab3

#endif /* LAB3_TASK2 */
