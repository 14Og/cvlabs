#ifndef LAB3_TASK4
#define LAB3_TASK4

#include "Core.hpp"
#include "Base.hpp"

#include <vector>

namespace Lab3 {

class WrenchChecker {

	using Contour = std::vector<cv::Point>;

public:
	WrenchChecker(const std::filesystem::path aMediaPathname):
		mediaPathname(std::move(aMediaPathname)),
		wrenchTemplate(cv::imread(mediaPathname / "template.jpg", cv::IMREAD_ANYCOLOR))

	{
		cv::cvtColor(wrenchTemplate, wrenchTemplate, cv::COLOR_BGR2GRAY);
	}

	void ckeckContours()
	{
		auto test1		   = cv::imread(mediaPathname / "test_1.jpg", cv::IMREAD_ANYCOLOR);
		auto test2		   = cv::imread(mediaPathname / "test_2.jpg", cv::IMREAD_ANYCOLOR);
		auto wrenchContour = getTemplateContour();

		matchContours(wrenchContour, test1);
		matchContours(wrenchContour, test2);
	}

private:
	auto getTemplateContour() -> Contour
	{
		std::vector<Contour> contours;
		cv::Mat workImage;
		auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
		cv::threshold(wrenchTemplate, workImage, 220, 255, cv::THRESH_BINARY);
		cv::erode(wrenchTemplate, wrenchTemplate, kernel);
		cv::dilate(wrenchTemplate, wrenchTemplate, kernel);
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		return contours[0];
	}

	void matchContours(Contour &aTemplate, cv::Mat &aTest)
	{
		cv::Mat grayImage;
		cv::cvtColor(aTest, grayImage, cv::COLOR_BGR2GRAY);
		auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {3, 3});
		cv::bitwise_not(grayImage, grayImage);
        cv::threshold(grayImage, grayImage, 140, 255, cv::THRESH_BINARY);
		cv::erode(grayImage, grayImage, kernel);
		cv::dilate(grayImage, grayImage, kernel);
		cv::imshow("Grayscale", grayImage);
		cv::waitKey();
		std::vector<Contour> contours;
		cv::findContours(grayImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		cv::drawContours(aTest, contours, -1, cv::Scalar(0, 0, 255), 2);
		for (const auto &contour : contours) {
		    auto match = cv::matchShapes(aTemplate, contour, cv::CONTOURS_MATCH_I2, 0);
		    if (match > 2.) {
		        cv::polylines(aTest, contour, 0, cv::Scalar(0, 0, 255), 2);
		    }
		    else {
		        cv::polylines(aTest, contour, 0, cv::Scalar(0, 255, 0), 2);
		    }
		}
		cv::imshow("Matching", aTest);
		cv::waitKey();
	}

private:
	std::filesystem::path mediaPathname;
	cv::Mat wrenchTemplate;
};

} // namespace Lab3

#endif /* LAB3_TASK4 */
