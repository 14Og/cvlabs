#ifndef LAB4_CONVOLUTION
#define LAB4_CONVOLUTION

#include "CoreHeaders.hpp"
#include "Utils.hpp"
#include "Defines.hpp"

namespace Lab4 {

class Convolution {
public:
	Convolution() = default;

	void testConv(const cv::Mat &aSrc, cv::Mat &aKernel, cv::Mat &aDest)
	{
		ComplexMat imgComplexSrc;
        ComplexMat complexKernel;
		gray2Complex(aSrc, imgComplexSrc);
        gray2Complex(aKernel, complexKernel);

		convolveFFT(imgComplexSrc, complexKernel, aDest);
		getReChannel(aDest, aDest);
		cv::normalize(aDest, aDest, 0, 1, cv::NORM_MINMAX);
	}
private:

	void convolveFFT(const cv::Mat &aSrc, const cv::Mat &aKernel, cv::Mat &aDst)
	{
		cv::Mat padded;
		cv::copyMakeBorder(
			aSrc, padded, aKernel.rows / 2, aKernel.rows / 2, aKernel.cols / 2, aKernel.cols / 2, cv::BORDER_REFLECT);

		cv::Mat temp(cv::Mat::zeros(padded.size(), padded.type()));
		aKernel.copyTo(temp(cv::Rect(0, 0, aKernel.cols, aKernel.rows)));

		cv::dft(padded, padded);
		cv::dft(temp, temp);

		cv::mulSpectrums(padded, temp, padded, 0);
		cv::dft(padded, padded, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

		aDst = padded(cv::Rect(aKernel.cols / 2, aKernel.rows / 2, aSrc.cols, aSrc.rows));
	}

};

} // namespace Lab4

#endif /* LAB4_CONVOLUTION */
