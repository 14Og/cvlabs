#ifndef LAB4_FTIMPL
#define LAB4_FTIMPL

#include "CoreHeaders.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <complex>
#include <numbers>
#include <array>
#include <vector>

#include <iostream>

namespace Lab4 {

using namespace std::complex_literals;

class FTImpl {

	static constexpr auto kPi = std::numbers::pi_v<double>;
	static constexpr auto kI  = 1i;

public:
	FTImpl() = default;

	void DFTtransform1D(const ComplexMat &aSrc, ComplexMat &aDst, bool aInverse = false)
	{
		assert(aSrc.rows == 1 || aSrc.cols == 1);

		size_t N = std::max(aSrc.rows, aSrc.cols);

		if (N != dftCoefficients.size())
			updateCoefs(N, aInverse);

		int k = 0;
		for (auto &dst : aDst) {
			dst	  = 0;
			int n = 0;
			for (const auto &src : aSrc) {
				auto coefId = (n * k) % N;
				dst += src * dftCoefficients.at(coefId);
				++n;
			}
			++k;
		}
		if (aInverse)
			aDst /= N;
	}

	void DFTtransform2D(const cv::Mat &aSrc, cv::Mat &aDst, bool aInverse = false)
	{
		ComplexMat result(aSrc.size());
		for (auto rowId = 0; rowId < aSrc.rows; ++rowId) {
			ComplexMat srcRow = aSrc.row(rowId);
			ComplexMat dftRow = result.row(rowId);
			DFTtransform1D(srcRow, dftRow, aInverse);
		}

		for (auto colId = 0; colId < aSrc.cols; ++colId) {
			ComplexMat srcCol = result.col(colId).clone();
			ComplexMat dftCol = result.col(colId);
			DFTtransform1D(srcCol, dftCol, aInverse);
		}

		aDst = result;
		dftCoefficients.clear();
	}

	void FFTtransform1D(const ComplexMat &aSrc, ComplexMat &aDest, bool aInverse = false)
	{
		assert(aSrc.rows == 1); // 1D shape. Transpose for second stage

		auto N = aSrc.cols;

		assert((N & (N - 1)) == 0); // N is power of 2

		if (N == 1)
			return;

		// Divide
		ComplexMat even(cv::Size(N / 2, 1));
		ComplexMat odd(cv::Size(N / 2, 1));
		int i = 0;
		for (auto iter = aSrc.begin(); iter != aSrc.end(); iter += 2, ++i) {
			even.at<Complex>(0, i) = *iter;
			odd.at<Complex>(0, i) = *(iter + 1);
		}

		// Conquer
		FFTtransform1D(even, even, aInverse);
		FFTtransform1D(odd, odd, aInverse);

		// Combine
		updateCoefs(N, aInverse);
		for (int i = 0; i < N / 2; ++i) {
			auto product					= dftCoefficients.at(i) * odd.at<Complex>(0, i);
			aDest.at<Complex>(0, i)			= even.at<Complex>(0, i) + product;
			aDest.at<Complex>(0, i + N / 2) = even.at<Complex>(0, i) - product;
		}
	}

	void FFTtransform2D(const cv::Mat  &aSrc, cv::Mat &aDest, bool aInverse = false)
	{
		ComplexMat result = aSrc.clone();
		createFFTBorders(result, result);
		for (auto rowId = 0; rowId < result.rows; ++rowId) {
			ComplexMat dftRow = result.row(rowId);
			FFTtransform1D(dftRow, dftRow, aInverse);
		}
		for (auto colId = 0; colId < result.cols; ++colId) {
			ComplexMat srcCol = result.col(colId).t();
			ComplexMat dftCol(srcCol.size());
			FFTtransform1D(srcCol, dftCol, aInverse);
			result.col(colId) = dftCol.t();
		}
		if (aInverse)
			result /= (aSrc.cols * aSrc.rows);

		aDest = result;
		dftCoefficients.clear();
	}

	void parallelFFT2D()
	{
	}

	void createFFTBorders(const cv::Mat &aSrc, cv::Mat &aDest)
	{
		auto sizeFunctor
			= [](int x) { return static_cast<int>(std::pow(2, std::ceil(std::log2(static_cast<double>(x))))); };
		cv::Size srcSize = aSrc.size();
		cv::Mat padded;
		cv::copyMakeBorder(aSrc, padded, 0, sizeFunctor(srcSize.height) - aSrc.rows, 0,
			sizeFunctor(srcSize.width) - aSrc.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
		aDest = padded;
	}

private:
	void updateCoefs(int aNewSize, bool aInverse)
	{
		dftCoefficients.resize(aNewSize);
		auto sign	  = aInverse ? 1 : -1;
		const auto cW = std::exp((sign * 2 * kPi / aNewSize) * kI);
		Complex w	  = 1.;
		for (auto &coef : dftCoefficients) {
			coef = w;
			w *= cW;
		}
	}

private:
	std::vector<Complex> dftCoefficients;
};
} // namespace Lab4

#endif /* LAB4_FTIMPL */
