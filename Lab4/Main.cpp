#ifndef LAB4_MAIN
#define LAB4_MAIN

#include "Defines.hpp"
#include "Utils.hpp"
#include "Lab2/ExecTimeCounter.hpp"

#include "FTImpl.hpp"
#include "Convolution.hpp"
#include "FreqFilter.hpp"
#include "CarNumbers.hpp"

using namespace Lab4;

int main(int, char **, char **)
{

	FTImpl ftImpl;
	auto mediaPath = Defines::resourcesPath / "Lab4";
	auto image	   = cv::imread(mediaPath / "resized_doom.jpg", cv::IMREAD_GRAYSCALE);

	//! Fourier transform. Paragraphs 1-3
	cv::Mat cvDft, customDft, customFft;
	cv::Mat cvDftMag, customDftMag, customFftMag;
	cv::Mat inverseCustomDft, inverseOpencvDft, inverseCustomFft;

	ComplexMat imageComplex;
	gray2Complex(image, imageComplex);

	//! Forward dft
	Lab2::execTimeCounter([&] { ftImpl.DFTtransform2D(imageComplex, customDft); }, "custom DFT");
	Lab2::execTimeCounter([&] { ftImpl.FFTtransform2D(imageComplex, customFft); }, "custom FFT");
	Lab2::execTimeCounter([&] { cv::dft(imageComplex, cvDft); }, "opencv DFT");

	magnitude(customDft, customDftMag);
	magnitude(cvDft, cvDftMag);
	magnitude(customFft, customFftMag);
	cvDftMag.convertTo(cvDftMag, CV_8UC1, 255);
	customDftMag.convertTo(customDftMag, CV_8UC1, 255);
	customFftMag.convertTo(customFftMag, CV_8UC1, 255);

	// !Inverse dft
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
	cv::waitKey();
	cv::destroyAllWindows();


	//! Convolution. Paragraph 7
	cv::Mat boxFilter(3, 3, CV_64F, cv::Scalar(1. / 9));
	cv::Mat sobelFilterX	= (cv::Mat_<double>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
	cv::Mat sobelFilterY	= (cv::Mat_<double>(3, 3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
	cv::Mat laplacianFilter = (cv::Mat_<double>(3, 3) << 0, 1, 0, 1, -4, 1, 0, 1, 0);

	Convolution conv;
	cv::Mat boxFilterResult, sobelFilterXResult, sobelFilterYResult, laplacianFilterResult;
	conv.testConv(image, boxFilter, boxFilterResult);
	conv.testConv(image, sobelFilterX, sobelFilterXResult);
	conv.testConv(image, sobelFilterY, sobelFilterYResult);
	conv.testConv(image, laplacianFilter, laplacianFilterResult);

	cv::imshow("Box filter", boxFilterResult);
	cv::imshow("Sobel filter X", sobelFilterXResult);
	cv::imshow("Sobel filter Y", sobelFilterYResult);
	cv::imshow("Laplacian filter", laplacianFilterResult);
	cv::waitKey();
	cv::destroyAllWindows();

	//! Frequency filtering. Paragraph 8
	FreqFilter filter;
	auto image2 = cv::imread(mediaPath / "bones.png", cv::IMREAD_GRAYSCALE);
	cv::resize(image2, image2, cv::Size(450, 450), cv::INTER_LINEAR);
	cv::Mat highPassSpectrum, highPassImage;
	cv::Mat lowPassSpectrum, lowPassImage;

	filter.highPassFilter(image2, highPassImage, highPassSpectrum, 50);
	filter.lowPassFilter(image2, lowPassImage, lowPassSpectrum, 50);

	cv::imshow("Original", image2);

	cv::imshow("High-pass", highPassImage);
	cv::imshow("High-pass spectrum", highPassSpectrum);
	cv::imshow("Low-pass", lowPassImage);
	cv::imshow("Low-pass spectrum", lowPassSpectrum);
	cv::waitKey();
	cv::destroyAllWindows();


	//! Template matching. Paragraph 9
	CarNumbers carNumbers(mediaPath / "CarNumbers");

	carNumbers.matchTemplates();
	cv::waitKey();
	cv::destroyAllWindows();

}

#endif /* LAB4_MAIN */
