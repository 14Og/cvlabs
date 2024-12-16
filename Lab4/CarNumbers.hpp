#ifndef LAB4_CARNUMBERS
#define LAB4_CARNUMBERS

#include "Core.hpp"
#include "Utils.hpp"
#include "Defines.hpp"

#include <filesystem>
#include <ranges>

namespace Lab4 {
class CarNumbers {

	using Iter = std::filesystem::directory_iterator;

public:
	CarNumbers(const std::filesystem::path aNumbersPath)
	{
		std::ranges::for_each(Iter(std::move(aNumbersPath)), [this](const auto &aDirEntry) {
			auto image = (cv::imread(aDirEntry.path().string(), cv::IMREAD_GRAYSCALE));
			allNumbers.push_back(image);
		});
		cv::imshow("All numbers", allNumbers);
		createTemplates();
	}

	void matchTemplates()
	{
		ComplexMat complexImage;
		for (const auto &templateMask : templates) {
			ComplexMat complexTemplate;
			cv::Mat imageMag, templateMag;
			cv::Mat result;
			gray2Complex(templateMask, complexTemplate);
			gray2Complex(allNumbers, complexImage);

			cv::dft(complexImage, complexImage);
			cv::dft(complexTemplate, complexTemplate);
			magnitude(complexImage, imageMag);
			magnitude(complexTemplate, templateMag);

			cv::mulSpectrums(complexImage, complexTemplate, result, 0, true);
			cv::dft(result, result, cv::DFT_INVERSE);
			getReChannel(result, result);
			cv::normalize(result, result, 0, 1, cv::NORM_MINMAX);
			cv::Mat thresholded;
			cv::threshold(result, thresholded, 0.99, 1, cv::THRESH_BINARY);

			cv::imshow("Result", result);
			cv::imshow("Thresholded result", thresholded);

			cv::imshow("Image magnitude", imageMag);
			cv::imshow("Template magnitude", templateMag);
			cv::imshow("Template", templateMask);
			cv::waitKey(0);
		}
	}

private:
	void createTemplates()
	{
		cv::Mat templateMask = cv::Mat::zeros(allNumbers.size(), CV_8U);
		auto symbolTemplate	 = allNumbers(cv::Rect(27, 37, 35, 28)); // 00

		symbolTemplate.copyTo(templateMask(cv::Rect(0, 0, 35, 28)));

		templates.push_back(templateMask.clone());

		symbolTemplate = allNumbers(cv::Rect(4, 37, 23, 26)); // E
		templateMask   = cv::Mat::zeros(allNumbers.size(), CV_8U);
		symbolTemplate.copyTo(templateMask(cv::Rect(0, 0, 23, 26)));

		templates.push_back(templateMask.clone());

		symbolTemplate = allNumbers(cv::Rect(61, 37, 15, 26));
		templateMask   = cv::Mat::zeros(allNumbers.size(), CV_8U);
		symbolTemplate.copyTo(templateMask(cv::Rect(0, 0, 15, 26)));
		templates.push_back(templateMask.clone());
	}

private:
	cv::Mat allNumbers;
	std::vector<cv::Mat> templates;
};

} // namespace Lab4

#endif /* LAB4_CARNUMBERS */
