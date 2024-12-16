#ifndef LAB4_UTILS
#define LAB4_UTILS

#include "Core.hpp"

#include <array>
#include <iostream>

namespace Lab4 {

using Complex	 = std::complex<double>;
using ComplexMat = cv::Mat_<Complex>;
using Mat1D		 = cv::Mat1d;


void gray2Complex(const Mat1D &aSrc, ComplexMat &aDest)
{
	std::array<cv::Mat, 2> planes{Mat1D(aSrc), cv::Mat::zeros(aSrc.size(), CV_64F)};
	cv::merge(planes, aDest);
}

void gray2EmptyComplex(const Mat1D &aSrc, ComplexMat &aDest)
{
	std::array<cv::Mat, 2> planes{Mat1D(aSrc.size(), CV_64F), cv::Mat::zeros(aSrc.size(), CV_64F)};
	cv::merge(planes, aDest);
}

void rearrangeSpectrum(cv::Mat &aSrc)
{
	int cx = aSrc.cols / 2;
	int cy = aSrc.rows / 2;

	cv::Mat q0(aSrc, {0, 0, cx, cy});
	cv::Mat q1(aSrc, {cx, 0, cx, cy});
	cv::Mat q2(aSrc, {0, cy, cx, cy});
	cv::Mat q3(aSrc, {cx, cy, cx, cy});
	cv::Mat tmp;

	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
}

void magnitude(cv::Mat &aSrc, cv::Mat &aDst)
{
	std::array<cv::Mat, 2> planes;
	cv::split(aSrc, planes);
	auto &[re, im] = planes;
	cv::magnitude(re, im, aDst);
	aDst += cv::Scalar::all(1);
	cv::log(aDst, aDst);
	aDst = aDst(cv::Rect(0, 0, aDst.cols & -2, aDst.rows & -2));
	rearrangeSpectrum(aDst);
	cv::normalize(aDst, aDst, 0, 1, cv::NORM_MINMAX);
}

void getReChannel(const cv::Mat &aSrc, cv::Mat &aDest)
{
    cv::extractChannel(aSrc, aDest, 0);
}

} // namespace Lab4

#endif /* LAB4_UTILS */
