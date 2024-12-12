#ifndef LAB3_TASK4
#define LAB3_TASK4

#include "Core.hpp"
#include "Base.hpp"

#include <vector>

namespace Lab3 {

using Contour  = std::vector<cv::Point>;
using Contours = std::vector<Contour>;
class ThreshHelper {

public:
	ThreshHelper() = default;

public:
	void start(const std::string aPathname)
	{
		src = cv::imread(std::move(aPathname));
		cv::cvtColor(src, src, cv::COLOR_BGR2GRAY);

		updateImage();
		setupCallbacks();
		if (cv::waitKey() == Defines::kQuitKey)
			cv::destroyAllWindows();
	}

private:
	void updateImage()
	{
		auto workImage = src.clone();
		cv::threshold(src, workImage, thresh, maxValue, cv::THRESH_BINARY_INV);
		auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {9, 9});
		cv::erode(workImage, workImage, kernel);
		cv::dilate(workImage, workImage, kernel);

		Contours contours;
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

		auto clone = src.clone();
		cv::cvtColor(clone, clone, cv::COLOR_GRAY2BGR);
		cv::drawContours(clone, contours, -1, {0, 0, 255}, 2);
		cv::putText(clone, "Contours: " + std::to_string(contours.size()), {30, 30}, cv::FONT_HERSHEY_COMPLEX, 0.5,
			{0, 0, 0}, 2);
		cv::imshow("Contours", clone);

		// cv::imshow("Canny helper", workImage);
	}

	void setupCallbacks()
	{
		cv::createTrackbar("thresh", "Contours", &thresh, 255, onThreshChange, this);
		cv::createTrackbar("max value", "Contours", &maxValue, 255, onMaxValueChange, this);
	}

	static inline void onThreshChange(int aVal, void *aSelf)
	{
		auto self	 = reinterpret_cast<ThreshHelper *>(aSelf);

		self->thresh = aVal;
		self->updateImage();
	}

	static inline void onMaxValueChange(int aVal, void *aSelf)
	{
		auto self	   = reinterpret_cast<ThreshHelper *>(aSelf);

		self->maxValue = aVal;
		self->updateImage();
	}

private:
	cv::Mat src;

	int thresh{0};
	int maxValue{0};
};

class WrenchChecker {

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
		// matchContours(wrenchContour, test2);
	}

private:
	auto getTemplateContour() -> Contour
	{
		Contours contours;
		cv::Mat workImage;
		auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {5, 5});
		cv::threshold(wrenchTemplate, workImage, 252, 110, cv::THRESH_BINARY);
		cv::erode(wrenchTemplate, wrenchTemplate, kernel);
		cv::dilate(wrenchTemplate, wrenchTemplate, kernel);
		cv::findContours(workImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		return contours.at(0);
	}

	void matchContours(Contour &aTemplate, cv::Mat &aTest)
	{
		cv::Mat grayImage;
		cv::cvtColor(aTest, grayImage, cv::COLOR_BGR2GRAY);
		auto kernel = cv::getStructuringElement(cv::MORPH_RECT, {9, 9});
		cv::threshold(grayImage, grayImage, 250, 110, cv::THRESH_BINARY_INV);
		cv::erode(grayImage, grayImage, kernel);
		cv::dilate(grayImage, grayImage, kernel);
		Contours contours;
		cv::findContours(grayImage, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
		for (const auto &contour : contours) {
			auto match = cv::matchShapes(aTemplate, contour, cv::CONTOURS_MATCH_I2, 0);
			std::cout << "Contour match: " << match << std::endl;
			if (match > 0.2) {
				cv::polylines(aTest, contour, -1, cv::Scalar(0, 0, 255), 2);
			} else {
				cv::polylines(aTest, contour, -1, cv::Scalar(0, 255, 0), 2);
			}
		}
		cv::imshow("Matching", aTest);
		cv::waitKey();
	}

public:
	ThreshHelper helper;

private:
	std::filesystem::path mediaPathname;
	cv::Mat wrenchTemplate;
};

} // namespace Lab3

#endif /* LAB3_TASK4 */
