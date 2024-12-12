#ifndef LAB3_TASK3
#define LAB3_TASK3

#include "Core.hpp"
#include "Base.hpp"

#include <array>
#include <algorithm>

namespace Lab3 {

class RobotsFinder : public FinderBase {

	using Contours = std::vector<std::vector<cv::Point>>;

public:
	RobotsFinder(const std::string aMediaPathname): FinderBase(std::move(aMediaPathname)), helper(absPhotoPathnames)
	{
	}

	void findOnPhotos([[maybe_unused]] DisplayMode aMode) final
	{
		for (const auto &pathname : absPhotoPathnames) {
			auto sourceImage = cv::imread(pathname, cv::IMREAD_COLOR);
			std::array<Contours, 3> robotContours;
			robotContours.at(0) = findBlueContours(sourceImage);
			robotContours.at(1) = findGreenContours(sourceImage);
			robotContours.at(2) = findRedContours(sourceImage);

			auto lampContour	= findLamp(sourceImage);
            drawClosestRobot(sourceImage, robotContours, lampContour);
            auto windowName = "Robots: " + pathname.filename().string();
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
            auto windowName = "Robots: " + pathname.filename().string();
            while (videoCapture.read(frame)) {
                std::array<Contours, 3> robotContours;
                robotContours.at(0) = findBlueContours(frame);
                robotContours.at(1) = findGreenContours(frame);
                robotContours.at(2) = findRedContours(frame);

                auto lampContour = findLamp(frame);
                drawClosestRobot(frame, robotContours, lampContour);
                cv::imshow(windowName, frame);
                if (cv::waitKey(40) == Defines::kQuitKey)
                    break;
            }
            if (aMode == DisplayMode::SUBSEQUENTIAL) {
				cv::waitKey();
				cv::destroyWindow(windowName);
			} else
				cv::waitKey(100);
        }
    }

private:
	auto findRedContours(cv::Mat &aSourceImage) -> Contours
	{
		cv::Mat hsvImage;
		cv::Mat workImage;
		Contours contours;
		cv::cvtColor(aSourceImage, hsvImage, cv::COLOR_BGR2HSV);

		auto redKernel = cv::getStructuringElement(cv::MORPH_RECT, {redMorphKernelSize, redMorphKernelSize});

		cv::Mat tempRedImage;
		cv::cvtColor(aSourceImage, tempRedImage, cv::COLOR_BGR2HSV);
		cv::cvtColor(aSourceImage, workImage, cv::COLOR_BGR2HSV);
		cv::inRange(hsvImage, upperRedMin, upperRedMax, workImage);
		cv::inRange(tempRedImage, LowerRedMin, LowerRedMax, tempRedImage);
		cv::bitwise_or(workImage, tempRedImage, workImage);
		cv::erode(workImage, workImage, redKernel);
		cv::dilate(workImage, workImage, redKernel);
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		cv::drawContours(aSourceImage, contours, -1, red, 2, cv::LINE_8);

		return contours;
	}

	auto findGreenContours(cv::Mat &aSourceImage) -> Contours
	{
		cv::Mat hsvImage;
		cv::Mat workImage;
		Contours contours;
		cv::cvtColor(aSourceImage, hsvImage, cv::COLOR_BGR2HSV);

		auto greenKernel = cv::getStructuringElement(cv::MORPH_RECT, {greenMorphKernelSize, greenMorphKernelSize});

		cv::inRange(hsvImage, greenMin, greenMax, workImage);
		cv::erode(workImage, workImage, greenKernel, {1,1});
		cv::dilate(workImage, workImage, greenKernel);
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		cv::drawContours(aSourceImage, contours, -1, green, 2, cv::LINE_8);

		return contours;
	}

	auto findBlueContours(cv::Mat &aSourceImage) -> Contours
	{
		cv::Mat hsvImage;
		cv::Mat workImage;
		Contours contours;
		cv::cvtColor(aSourceImage, hsvImage, cv::COLOR_BGR2HSV);

		auto blueKernel = cv::getStructuringElement(cv::MORPH_RECT, {blueMorphKernelSize, blueMorphKernelSize});

		cv::inRange(hsvImage, blueMin, blueMax, workImage);
		cv::erode(workImage, workImage, blueKernel);
		cv::dilate(workImage, workImage, blueKernel);
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		cv::drawContours(aSourceImage, contours, -1, blue, 2, cv::LINE_8);

		return contours;
	}

	auto findLamp(cv::Mat &aSourceImage) -> Contours
	{
		cv::Mat hsvImage;
		cv::Mat workImage;
		Contours contours;
		cv::cvtColor(aSourceImage, hsvImage, cv::COLOR_BGR2HSV);

		auto lampKernel = cv::getStructuringElement(cv::MORPH_RECT, {lampMorphKernelSize, lampMorphKernelSize});

		cv::inRange(hsvImage, lampMin, lampMax, workImage);
        cv::erode(workImage, workImage, lampKernel);
		cv::dilate(workImage, workImage, lampKernel);
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		return contours;
	}

	void drawClosestRobot(
		cv::Mat &aSourceImage, const std::array<Contours, 3> &aRobotContours, const Contours &aLampContours)
	{
		auto lampMoments = cv::moments(aLampContours.at(0));
		auto lampCenter	 = cv::Point2d(lampMoments.m10 / lampMoments.m00, lampMoments.m01 / lampMoments.m00);
		for (const auto &colorContours : aRobotContours) {
			auto closestContour
				= std::min_element(colorContours.begin(), colorContours.end(), [&](const auto &lhs, const auto &rhs) {
					  auto lhsMoments = cv::moments(lhs);
					  auto rhsMoments = cv::moments(rhs);
					  auto lhsCenter  = cv::Point2d(lhsMoments.m10 / lhsMoments.m00, lhsMoments.m01 / lhsMoments.m00);
					  auto rhsCenter  = cv::Point2d(rhsMoments.m10 / rhsMoments.m00, rhsMoments.m01 / rhsMoments.m00);
					  return cv::norm(lampCenter - lhsCenter) < cv::norm(lampCenter - rhsCenter);
				  });
			auto moment		  = cv::moments(*closestContour);
			auto closestPoint = cv::Point2d(moment.m10 / moment.m00, moment.m01 / moment.m00);
			cv::circle(aSourceImage, closestPoint, 3, cv::Scalar(0, 0, 0), -1);
            cv::circle(aSourceImage, lampCenter, 3, cv::Scalar(0, 0, 0), -1);
			cv::line(aSourceImage, lampCenter, closestPoint, cv::Scalar(0, 0, 0), 2);
		}
	}

public:
	HSVThresholdHelper helper;

private:
	cv::Scalar blueMin{92, 50, 50};
	cv::Scalar blueMax{120, 255, 255};
	int blueMorphKernelSize{5};
	cv::Scalar blue{255, 0, 0};

	cv::Scalar greenMin{40, 50, 80};
	cv::Scalar greenMax{80, 255, 255};
	int greenMorphKernelSize{9};
	cv::Scalar green{0, 255, 0};

	cv::Scalar upperRedMin{0, 80, 80};
	cv::Scalar upperRedMax{10, 255, 255};
	cv::Scalar LowerRedMin{160, 80, 80};
	cv::Scalar LowerRedMax{180, 255, 255};

	int redMorphKernelSize{5};
	cv::Scalar red{0, 0, 255};

	cv::Scalar lampMin{0, 0, 250};
	cv::Scalar lampMax{0, 0, 255};
	int lampMorphKernelSize{3};
	cv::Scalar lamp{255, 255, 255};
};

}; // namespace Lab3

#endif /* LAB3_TASK3 */
