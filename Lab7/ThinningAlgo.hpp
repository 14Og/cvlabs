#ifndef _LAB7_THINNINGALGO_HPP_
#define _LAB7_THINNINGALGO_HPP_

#include "CoreHeaders.hpp"

#include <vector>
#include <cstdint>

namespace Thinning {
enum class Step : uint8_t {
	FIRST = 1,
	SECOND,
};

// converts bool into char which's bit representation is : 0|1 0|1 0|1 ...
uint8_t bool2CharPixels(std::vector<bool> &aPixels)
{

	if (aPixels.size() != 8)
		throw "Only 8 neighbours have to be observed";

	uint8_t result = 0;
	for (size_t i = 0; i < aPixels.size(); ++i) {
		result |= (aPixels[i] << (7 - i));
	}

	return result;
}

int countPixelSwitches(uint8_t aPixels)
{
	int count = 0;
	for (int i = 7; i > 0; --i) {
		// extract the two bits at positions i and i+1
		int bit0 = (aPixels >> i) & 1; // Bit at position i
		int bit1 = (aPixels >> (i - 1)) & 1; // Bit at position i+1

		// check for "01" pattern: bit0 = 0 and bit1 = 1
		if (bit0 == 0 && bit1 == 1) {
			count++;
		}
	}

	// check transition between last and first bit
	if (!(aPixels & 1) && (aPixels >> 7 & 1))
		++count;

	return count;
}

int countWhitePixels(uint8_t aPixels)
{
	int count = 0;
	for (int i = 0; i < 8; ++i) {
		count += (aPixels >> i) & 1;
	}

	return count;
}

void performStep(cv::Mat &aBinImage, Step aStep)
{
	if (aBinImage.channels() != 1)
		throw "Only grayscale binary images can be thinned";

	cv::Mat mask = cv::Mat::ones(aBinImage.size(), aBinImage.type());

	for (int i = 1; i < aBinImage.cols - 1; ++i) {
		for (int j = 1; j < aBinImage.rows - 1; ++j) {
			// (0)
			if (aBinImage.at<uint8_t>(j, i) != 1)
				continue;
			std::vector<bool> pixels{
				aBinImage.at<bool>(j - 1, i),
				aBinImage.at<bool>(j - 1, i + 1),
				aBinImage.at<bool>(j, i + 1),
				aBinImage.at<bool>(j + 1, i + 1),
				aBinImage.at<bool>(j + 1, i),
				aBinImage.at<bool>(j + 1, i - 1),
				aBinImage.at<bool>(j, i - 1),
				aBinImage.at<bool>(j - 1, i - 1),
			};
			auto pixelBits = bool2CharPixels(pixels);

			// (1)
			auto whitePix = countWhitePixels(pixelBits);
			// std::cout << "row: " << j << " col: " << i << " white pixels: " << whitePix << std::endl;
			if (whitePix < 2 || whitePix > 6)
				continue;

			// (2)
			auto pixelSwitches = countPixelSwitches(pixelBits);
			// std::cout << "row: " << j << " col: " << i << " pixel switches: " << pixelSwitches << std::endl;
			if (pixelSwitches != 1) {
				// std::cout << "pixels switches: " << pixelSwitches << std::endl;
				continue;
			}

			if (aStep == Step::FIRST) {
				// (3)
				if (pixels[0] * pixels[2] * pixels[4] != 0)
					continue;
				// (4)
				if (pixels[2] * pixels[4] * pixels[6] != 0)
					continue;
			} else {
				// (3)
				if (pixels[0] * pixels[2] * pixels[6] != 0)
					continue;
				// (4)
				if (pixels[0] * pixels[4] * pixels[6] != 0)
					continue;
			}
			mask.at<uint8_t>(j, i) = 0;
		}
	}
	aBinImage &= mask;
}

void performFull(cv::Mat &aImage)
{
	aImage /= 255;
	auto diff = aImage.clone();
	cv::Mat temp = cv::Mat::zeros(aImage.size(), aImage.type());
	for(int i = 0; ; ++i) {
		performStep(aImage, Step::FIRST);
		performStep(aImage, Step::SECOND);
		cv::absdiff(aImage, temp, diff);
		aImage.copyTo(temp);
		auto diffSum = cv::sum(diff);
		if (diffSum.val[0] == 0)
			break;
	}
	aImage *= 255;
}

} // namespace Thinning

#endif /* _LAB7_THINNINGALGO_HPP_ */
