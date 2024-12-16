#ifndef LAB4_TESTS
#define LAB4_TESTS

#include "FTImpl.hpp"

#include "Lab2/ExecTimeCounter.hpp"

#include "Defines.hpp"

using namespace Lab4;

void testDft(cv::Mat &aImgSource)
{
	ComplexMat imgComplexSrc;
	gray2Complex(aImgSource, imgComplexSrc);
	ComplexMat imgComplexDft(cv::Mat::zeros(imgComplexSrc.size(), imgComplexSrc.type()));
	ComplexMat cvDft = imgComplexDft.clone();

	FTImpl ft;

	for (auto rowId = 0; rowId < imgComplexSrc.rows; ++rowId) {
		ComplexMat rowSrc = imgComplexSrc.row(rowId);
		ComplexMat rowDst = imgComplexDft.row(rowId);
		ComplexMat cvDst  = cvDft.row(rowId);

		ft.DFTtransform1D(rowSrc, rowDst);
		cv::dft(rowSrc, cvDst, cv::DFT_COMPLEX_OUTPUT);

		for (auto colId = 0; colId < rowSrc.cols; ++colId) {
			auto &x = imgComplexDft.at<Complex>(rowId, colId);
			auto &y = cvDft.at<Complex>(rowId, colId);
			std::cout << x << "\t" << y << "\t" << (x - y) << std::endl;
		}
	}

	std::cout << "Second transform stage\n";

	for (auto colId = 0; colId < imgComplexSrc.cols; ++colId) {
		ComplexMat cvColSrc = cvDft.col(colId).clone();
		ComplexMat myColSrc = imgComplexDft.col(colId).clone();

		ComplexMat cvColDst = cvDft.col(colId);
		ComplexMat myColDst = imgComplexDft.col(colId);

		ft.DFTtransform1D(myColSrc, myColDst);
		cv::dft(cvColSrc, cvColDst, cv::DFT_COMPLEX_OUTPUT | cv::DFT_COMPLEX_INPUT);

		for (auto rowId = 0; rowId < cvColSrc.rows; ++rowId) {
			auto &x = imgComplexDft.at<Complex>(rowId, colId);
			auto &y = cvDft.at<Complex>(rowId, colId);
			std::cout << x << "\t" << y << "\t" << (x - y) << std::endl;
		}
	}
}

void testFFT(cv::Mat &aImgSource)
{
	FTImpl ft;

	ComplexMat imgComplexSrc;

	gray2Complex(aImgSource, imgComplexSrc);
	ft.createFFTBorders(imgComplexSrc, imgComplexSrc);
	ComplexMat imgComplexDft(cv::Mat::zeros(imgComplexSrc.size(), imgComplexSrc.type()));
	ComplexMat cvDft = imgComplexDft.clone();

	for (auto rowId = 0; rowId < imgComplexSrc.rows; ++rowId) {
		ComplexMat rowSrc = imgComplexSrc.row(rowId);
		ComplexMat rowDst = imgComplexDft.row(rowId);
		ComplexMat cvDst  = cvDft.row(rowId);
		ft.FFTtransform1D(rowSrc, rowDst);
		cv::dft(rowSrc, cvDst, cv::DFT_COMPLEX_OUTPUT);

		for (auto colId = 0; colId < rowSrc.cols; ++colId) {
			auto &x = imgComplexDft.at<Complex>(rowId, colId);
			auto &y = cvDft.at<Complex>(rowId, colId);
			std::cout << x << "\t" << y << "\t" << (x - y) << std::endl;
		}
	}
}

int main(int, char **, char **)
{

	FTImpl ftImpl;
	auto mediaPath = Defines::resourcesPath / "Lab4";
	auto image	   = cv::imread(mediaPath / "resized_doom.jpg", cv::IMREAD_GRAYSCALE);
	// cv::resize(image, image, {160, 90});

	// testFFT(image);

	cv::Mat cvDft, customDft, customFft;
	cv::Mat cvDftMag, customDftMag, customFftMag;
	cv::Mat inverseCustomDft, inverseOpencvDft, inverseCustomFft;

	ComplexMat imageComplex;
	gray2Complex(image, imageComplex);

	Lab2::execTimeCounter([&] { ftImpl.DFTtransform2D(imageComplex, customDft); }, "custom DFT");
	Lab2::execTimeCounter([&] { ftImpl.FFTtransform2D(imageComplex, customFft); }, "custom FFT");
	Lab2::execTimeCounter([&] { cv::dft(imageComplex, cvDft); }, "opencv DFT");

	magnitude(customDft, customDftMag);
	magnitude(cvDft, cvDftMag);
	magnitude(customFft, customFftMag);
	cvDftMag.convertTo(cvDftMag, CV_8UC1, 255);
	customDftMag.convertTo(customDftMag, CV_8UC1, 255);
	customFftMag.convertTo(customFftMag, CV_8UC1, 255);

	Lab2::execTimeCounter([&] { ftImpl.DFTtransform2D(customDft, inverseCustomDft, true); }, "custom inverse DFT");
	Lab2::execTimeCounter([&] { ftImpl.FFTtransform2D(customFft, inverseCustomFft, true); }, "custom inverse FFT");
	Lab2::execTimeCounter([&] { cv::dft(cvDft, inverseOpencvDft, cv::DFT_INVERSE); }, "opencv inverse DFT");

	getReChannel(inverseCustomDft, inverseCustomDft);
	getReChannel(inverseOpencvDft, inverseOpencvDft);
	getReChannel(inverseCustomFft, inverseCustomFft);
	cv::normalize(inverseCustomDft, inverseCustomDft, 0, 1, cv::NORM_MINMAX);
	cv::normalize(inverseOpencvDft, inverseOpencvDft, 0, 1, cv::NORM_MINMAX);
	cv::normalize(inverseCustomFft, inverseCustomFft, 0, 1, cv::NORM_MINMAX);

	cv::imshow("custom dft", customDftMag);
	cv::imshow("opencv dft", cvDftMag);
	cv::imshow("custom fft", customFftMag);

	cv::imshow("custom inverse dft", inverseCustomDft);
	cv::imshow("opencv inverse dft", inverseOpencvDft);
	cv::imshow("custom inverse fft", inverseCustomFft(cv::Rect(0, 0, image.cols, image.rows)));

	if (cv::waitKey() == Defines::kQuitKey)
		cv::destroyAllWindows();
}

#endif /* LAB4_TESTS */
