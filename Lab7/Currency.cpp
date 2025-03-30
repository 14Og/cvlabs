#ifndef _LAB7_CURRENCY_CPP_
#define _LAB7_CURRENCY_CPP_

#include "Core.hpp"
#include "Defines.hpp"

#include <vector>
#include <algorithm>
#include <strstream>
#include <array>

double getSaturation(cv::Mat src)
{
	cv::Mat hsvImage;
	cv::cvtColor(src, hsvImage, cv::COLOR_BGR2HSV);

	// H, S, V
	std::array<cv::Mat, 3> channels; 
	cv::split(hsvImage, channels.data());

	auto meanHue	= cv::mean(channels[1]);

	return meanHue.val[0];
}

double saturateDistance(cv::Mat src, cv::Mat sample)
{
	auto src_sat	= getSaturation(src);
	auto sample_sat = getSaturation(sample);
	return std::abs(src_sat - sample_sat);
}

void performCurrency(cv::Mat &aImage, cv::Mat &aCopper, cv::Mat &aNickel)
{

	cv::Mat gray;
	cv::cvtColor(aImage, gray, cv::COLOR_BGR2GRAY);

	auto absDist = saturateDistance(aCopper, aNickel);

	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, 20, 100, 30, 20, 50);
	for (const auto &circle : circles) {
		cv::Point center = cv::Point(circle[0], circle[1]);
		int radius		 = circle[2];

		cv::Rect roi(circle[0] - circle[2], circle[1] - circle[2], circle[2] * 2, circle[2] * 2);
		cv::Mat img_roi		  = aImage(roi);

		auto copperValue	  = saturateDistance(img_roi, aCopper);
		auto nickelPercentage = 100 * copperValue / absDist;
		auto copPercentage	  = 100 - nickelPercentage;

		auto copStr			  = std::to_string(copPercentage);
		auto nickStr		  = std::to_string(nickelPercentage);

		copStr				  = copStr.substr(0, copStr.find('.') + 3);
		nickStr				  = nickStr.substr(0, nickStr.find('.') + 3);

		cv::circle(aImage, center, radius, (copPercentage > 80.0) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 255, 0), 3,
			cv::LINE_8);
		cv::putText(aImage, copStr, center + cv::Point(-30, 10), cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 0, 0), 2);
		cv::putText(aImage, nickStr, center + cv::Point(-30, -10), cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 0, 0), 2);
	}

	cv::resize(aImage, aImage, aImage.size() * 2);
	cv::imshow("Money", aImage);

	cv::waitKey();
}

int main(void)
{
	auto moneyPath = Defines::resourcesPath / "Lab7" / "Photo" / "Money";

	auto image	   = cv::imread(moneyPath / "test.jpg");
	auto copper	   = cv::imread(moneyPath / "copper_pattern.jpg");
	auto nickel	   = cv::imread(moneyPath / "nickel_pattern.jpg");

	performCurrency(image, copper, nickel);

	cv::destroyAllWindows();
}

#endif /* _LAB7_CURRENCY_CPP_ */
