#ifndef LAB4_FREQFILTER
#define LAB4_FREQFILTER

#include "CoreHeaders.hpp"
#include "Defines.hpp"
#include "FTImpl.hpp"
#include "Utils.hpp"

namespace Lab4 {

class FreqFilter {
public:
    FreqFilter() = default;
    

    void highPassFilter(cv::Mat &aSource, cv::Mat &aDest, cv::Mat &aSpectrum, int aRadius)
    {
        ComplexMat imgComplexSrc;
        gray2Complex(aSource, imgComplexSrc);
        cv::dft(imgComplexSrc, aSpectrum, cv::DFT_COMPLEX_OUTPUT);
        auto origSpectrum = aSpectrum.clone();
        rearrangeSpectrum(origSpectrum);
        magnitude(aSpectrum, aSpectrum);

        cv::Mat mask(cv::Mat::ones(origSpectrum.size(), CV_8U));
        cv::circle(mask, cv::Point(origSpectrum.cols / 2, origSpectrum.rows / 2), aRadius, cv::Scalar(0), -1);
        cv::bitwise_xor(origSpectrum, origSpectrum, origSpectrum, mask);
        cv::bitwise_xor(aSpectrum, aSpectrum, aSpectrum, mask);

        rearrangeSpectrum(origSpectrum);
        cv::dft(origSpectrum, aDest, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        getReChannel(aDest, aDest);
        cv::normalize(aDest, aDest, 0, 1, cv::NORM_MINMAX);
    }

    void lowPassFilter(cv::Mat &aSource, cv::Mat &aDest, cv::Mat &aSpectrum, int aRadius)
    {
        ComplexMat imgComplexSrc;
        gray2Complex(aSource, imgComplexSrc);
        cv::dft(imgComplexSrc, aSpectrum, cv::DFT_COMPLEX_OUTPUT);
        auto origSpectrum = aSpectrum.clone();
        rearrangeSpectrum(origSpectrum);
        magnitude(aSpectrum, aSpectrum);

        cv::Mat mask(cv::Mat::zeros(origSpectrum.size(), CV_8U));
        cv::circle(mask, cv::Point(origSpectrum.cols / 2, origSpectrum.rows / 2), aRadius, cv::Scalar(255), -1);
        cv::bitwise_xor(origSpectrum, origSpectrum, origSpectrum, mask);
        cv::bitwise_xor(aSpectrum, aSpectrum, aSpectrum, mask);

        rearrangeSpectrum(origSpectrum);
        cv::dft(origSpectrum, aDest, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);
        getReChannel(aDest, aDest);
        cv::normalize(aDest, aDest, 0, 1, cv::NORM_MINMAX);
    }


};

}

#endif /* LAB4_FREQFILTER */
