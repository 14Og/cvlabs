#ifndef LAB1_LAB1
#define LAB1_LAB1

#include "Core.hpp"
#include "Defines.hpp"

#include <array>
#include <algorithm>
#include <cmath>
#include <numbers>
#include <utility>
#include <filesystem>
#include <thread>
#include <memory>

using Defines::resourcesPath;

namespace Lab1 {

class VacuumCleanerAnimation {

public:
	static constexpr auto kSampleRate		 = 10000;
	static constexpr auto kRadToDeg			 = 180 / std::numbers::pi_v<float>;
	static constexpr auto kDegToRad			 = std::numbers::pi_v<float> / 180;
	static constexpr auto kSinStart			 = 100;

private:
	VacuumCleanerAnimation(const VacuumCleanerAnimation &)			  = delete;
	VacuumCleanerAnimation &operator=(const VacuumCleanerAnimation &) = delete;
	VacuumCleanerAnimation(const std::string aVacuumCleaner, const std::string aBackground):
		cleaner(cv::imread(resourcesPath / aVacuumCleaner)),
		canvas(cv::imread(resourcesPath / aBackground))

	{
		cv::resize(canvas, canvas, cv::Size(1280, 720), 0, 0, cv::INTER_CUBIC);
		generateConstants();
		float xCoordinate = kSinStart;
		auto xShift		  = static_cast<float>(canvas.cols - 2 * kSinStart) / kSampleRate;
		std::generate(sinPoints.begin(), sinPoints.end(), [&] {
			cv::Point2f point(xCoordinate, yStart - amplitude * std::sin(freq * (xCoordinate - kSinStart)));
			xCoordinate += xShift;
			return point;
		});
		putSinusPoints();
	}
public:
	static inline void animate()
	{
		VacuumCleanerAnimation animation("vacuum_cleaner_chopped.png", "white_background.jpg");
		animation.emit(1);
		cv::destroyAllWindows();
	}

private:
	void generateConstants()
	{
		xStart	  = 0;
		yStart	  = canvas.rows / 2;
		amplitude = yStart - 100;
		freq	  = 2 * std::numbers::pi_v<float> / ((canvas.cols - 2 * kSinStart) / 3);
	}

	auto rad2deg(auto aRad)
	{
		return aRad * kRadToDeg;
	}

	auto deg2rad(auto aDeg)
	{
		return aDeg * kDegToRad;
	}

	void putSinusPoints()
	{
		for (auto i = 1; i < static_cast<decltype(i)>(sinPoints.size()); ++i) {
			cv::line(canvas, sinPoints[i - 1], sinPoints[i], cv::Scalar(0, 0, 255), 2);
		}
	}

	float getTangentAngle(const cv::Point2f &aPoint)
	{
		auto derivative = amplitude * freq * std::cos(freq * (aPoint.x - kSinStart));
		auto rad		= std::atan(derivative);
		auto deg		= rad2deg(rad) - 90;
		return deg;
	}

	cv::Mat getRotatedResizedImage(cv::Mat &aSrc, float aAngle, int aResizeFactor)
	{
		cv::Point2f center(aSrc.cols / 2., aSrc.rows / 2.);
		auto rot = cv::getRotationMatrix2D(center, aAngle, 1.0);
		cv::Mat rotated;
		cv::warpAffine(aSrc, rotated, rot, aSrc.size());
		cv::resize(rotated, rotated, cv::Size(rotated.rows / aResizeFactor, rotated.cols / aResizeFactor));
		return rotated;
	}

	void putRotatedImage(cv::Mat &aCanvas, cv::Mat &aImage, cv::Point2f &aPoint)
	{
		cv::Mat mask(cv::Mat::zeros(aImage.size(), aImage.type()));
		cv::circle(mask, cv::Point(mask.cols / 2, mask.rows / 2), std::min(mask.cols, mask.rows) / 2,
			cv::Scalar(255, 255, 255), -1);
		auto x = aPoint.x - aImage.cols / 2;
		auto y = aPoint.y - aImage.rows / 2;

		cv::Rect roi(cv::Point(x, y), aImage.size());
		cv::Mat destRoi = aCanvas(roi);
		aImage.copyTo(destRoi, mask);
	}

	void emit(int aDelay = 10)
	{
		auto cleanCanvas = canvas.clone();
		for (size_t i = 0; i < sinPoints.size(); ++i) {
			canvas		 = cleanCanvas.clone();
			auto angle	 = getTangentAngle(sinPoints[i]);
			auto rotated = getRotatedResizedImage(cleaner, angle, 3);
			putRotatedImage(canvas, rotated, sinPoints[i]);
			if (i == sinPoints.size() / 2)
				cv::imwrite(resourcesPath / "middle.jpg", canvas);
			cv::imshow("Animation", canvas);
			if (cv::waitKey(aDelay) == Defines::kQuitKey)
				break;
		}
	}

private:
	cv::Mat cleaner;
	cv::Mat canvas;

	uint16_t xStart;
	uint16_t yStart;
	uint16_t amplitude;
	float freq;

	std::array<cv::Point2f, kSampleRate> sinPoints;
};





}

#endif /* LAB1_LAB1 */
