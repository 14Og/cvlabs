#ifndef _LAB7_CURRENCY_CPP_
#define _LAB7_CURRENCY_CPP_

#include "ThinningAlgo.hpp"
#include "Core.hpp"
#include "Defines.hpp"

#include <vector>
#include <algorithm>

void setGamma(const cv::Mat &Img, cv::Mat &Res, const double gamma)
{
	static cv::Mat gammaTable = cv::Mat::ones(1, 256, CV_8U);
	static double gammaPrev			  = 1;

	if (gamma != gammaPrev) {
		auto p = gammaTable.ptr();
		for (int i = 0; i < 256; ++i) {
			p[i] = cv::saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);
		}
		gammaPrev = gamma;
	}

	cv::LUT(Img, gammaTable, Res);
}

double getSaturate(cv::Mat src)
{
	cv::Mat hsvImage;
	cv::cvtColor(src, hsvImage, cv::COLOR_BGR2HSV);

	cv::Mat channels[3];
	split(hsvImage, channels);

	cv::Mat sat		   = channels[1];

	cv::Scalar meanHue = mean(sat);

	return meanHue.val[0];
}

double saturateDistance(cv::Mat src, cv::Mat sample)
{
	double src_sat	  = getSaturate(src);
	double sample_sat = getSaturate(sample);
	return std::abs(src_sat - sample_sat);
}

void performCurrency(cv::Mat &aImage, cv::Mat &aCopper, cv::Mat &aNickel)
{
	// cv::Mat img	   = cv::imread(std::string(SOURCE_DIR) + "/images/money/1.jpg");
	// cv::Mat copper = cv::imread(std::string(SOURCE_DIR) + "/images/money/copper.jpg");
	// cv::Mat nikel  = cv::imread(std::string(SOURCE_DIR) + "/images/money/nikel.jpg");

	cv::Mat gray;
	cv::cvtColor(aImage, gray, cv::COLOR_BGR2GRAY);

	double absDist = saturateDistance(aCopper, aNickel);

	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(gray, circles, cv::HOUGH_GRADIENT, 1, 20, 100, 30, 20, 50);
	for (const auto &circle: circles) {
		cv::Point center = cv::Point(circle[0], circle[1]);
		int radius		 = circle[2];

		cv::Rect roi(circle[0] - circle[2], circle[1] - circle[2], circle[2] * 2, circle[2] * 2);
		cv::Mat img_roi		= aImage(roi);

		double copper_val	= saturateDistance(img_roi, aCopper);
		double nik_per		= 100 * copper_val / absDist;
		double cop_per		= 100 - nik_per;

		std::string cop_str = std::format("La:{:.1f}", cop_per);
		std::string nik_str = std::format("Ni:{:.1f}", nik_per);

		cv::circle(
			aImage, center, radius, (cop_per > 80.0) ? cv::Scalar(255, 0, 0) : cv::Scalar(0, 255, 0), 3, cv::LINE_8);
		cv::putText(aImage, cop_str, center + cv::Point(-30, 10), cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 0, 0), 2);
		cv::putText(aImage, nik_str, center + cv::Point(-30, -10), cv::FONT_HERSHEY_DUPLEX, 0.5, CV_RGB(0, 0, 0), 2);
	}

    cv::resize(aImage, aImage, aImage.size() * 2);
	cv::imshow("Money", aImage);

	cv::waitKey();
}

int main(void)
{
	auto moneyPath = Defines::resourcesPath / "Lab7" / "Photo" / "Money";

    auto image = cv::imread(moneyPath / "test.jpg");
    auto copper = cv::imread(moneyPath / "copper_pattern.jpg");
    auto nickel = cv::imread(moneyPath / "nickel_pattern.jpg");

    performCurrency(image, copper, nickel);

	cv::destroyAllWindows();
}

#endif /* _LAB7_CURRENCY_CPP_ */
