#ifndef LAB2_FILTERS
#define LAB2_FILTERS

#include "Core.hpp"

#include <array>

namespace Lab2 {

class Filters {

public:
	Filters() = default;

	void customBoxFIlter(const cv::Mat &aSrc, cv::Mat &aDest, const cv::Size &aKernel)
	{
		auto size = aSrc.size();
		cv::Rect box(0, 0, aKernel.width, aKernel.height);
		cv::Mat tempBorderedImage;

		cv::copyMakeBorder(aSrc, tempBorderedImage, aKernel.height / 2, aKernel.height / 2, aKernel.width / 2,
			aKernel.width / 2, cv::BORDER_REFLECT); // border to handle corner pixels

		for (; box.y < size.height; ++box.y) {
			for (box.x = 0; box.x < size.width; ++box.x) {
				auto roi	  = tempBorderedImage(box);
				auto mean	  = cv::mean(roi);
				auto &destRoi = aDest.at<cv::Vec3b>(box.y, box.x);
				destRoi[0]	  = mean[0];
				destRoi[1]	  = mean[1];
				destRoi[2]	  = mean[2];
			}
		}
	}

	template<typename FilterLambda>
	void unsharpMask(FilterLambda &&aFilter, const cv::Mat &aSrc, cv::Mat &aDest, float aSharpness)
	{
		aFilter(); // applying given filter
		aDest = (aSrc - aDest) + aSharpness * aSrc;  // unsharp mask formula
	}

	void laplacianFilter(const cv::Mat &aSrc, cv::Mat &aDest)
	{
		std::array<int, 9> kernelData{1,1,1,1,-8,1,1,1,1};
		cv::Mat kernel(3,3, CV_32S, kernelData.data());
		cv::filter2D(aSrc, aDest, -1, kernel);
	}

	[[nodiscard]] auto calculateAccuracy(const cv::Mat &aImg1, const cv::Mat &aImg2) -> std::pair<cv::Mat, double>
	{
		cv::Mat diff;
		cv::absdiff(aImg1, aImg2, diff);
		auto diffSum = cv::sum(diff);
		return std::make_pair(diff, 100 - (diffSum[0] + diffSum[1] + diffSum[2]) / (aImg1.size().area() * 3 * 255));
	}
};
} // namespace Lab2

#endif /* LAB2_FILTERS */
