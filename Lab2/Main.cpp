#include "Filters.hpp"
#include "ExecTimeCounter.hpp"
#include "Defines.hpp"

#include "opencv2/intensity_transform.hpp"

#include <chrono>

int main()
{
	auto sourceImage = cv::imread(Defines::resourcesPath / "220px-Lenna.png");
	Lab2::Filters filters;

	// Paragraphs 1 - 3: custom box filter vs cv::blur

	auto customBoxFIlterImage = cv::Mat(sourceImage.size(), sourceImage.type());
	auto cvBlurImage		  = cv::Mat(sourceImage.size(), sourceImage.type());

	Lab2::ExecTimeCounter(
		[&]() { filters.customBoxFIlter(sourceImage, customBoxFIlterImage, {3, 3}); }, "customBoxFIlter");
	Lab2::ExecTimeCounter([&]() { cv::blur(sourceImage, cvBlurImage, {3, 3}); }, "cv::blur");
	auto [blurDiff, accuracy] = filters.calculateAccuracy(customBoxFIlterImage, cvBlurImage);
	std::cout << "Accuracy: " << accuracy << std::endl;

	cv::imshow("Source", sourceImage);
	cv::imshow("customBoxFIlter", customBoxFIlterImage);
	cv::imshow("cv::blur", cvBlurImage);
	cv::imshow("blurDiff", blurDiff);

	if (cv::waitKey() == Defines::kQuitKey) {
		cv::destroyAllWindows();
	}

	// Paragraph 4: cv::GaussianBlur vs cv::blur
	auto gaussianBlur = cv::Mat(sourceImage.size(), sourceImage.type());
	cv::GaussianBlur(sourceImage, gaussianBlur, {3, 3}, 2);
	auto [gaussDiff, gausAccuracy] = filters.calculateAccuracy(cvBlurImage, gaussianBlur);
	cv::Mat gaussLogDiff(gaussDiff.size(), gaussDiff.type());
	cv::intensity_transform::logTransform(gaussDiff, gaussLogDiff);
	std::cout << "Gaussian accuracy: " << gausAccuracy << std::endl;

	cv::imshow("cv::GaussianBlur", gaussianBlur);
	cv::imshow("cv::blur", cvBlurImage);
	cv::imshow("gaussDiff", gaussDiff);
	cv::imshow("gaussLogDiff", gaussLogDiff);

	if (cv::waitKey() == Defines::kQuitKey) {
		cv::destroyAllWindows();
	}

	// Paragraph 5: unsharp mask filter for box filter and gaussian filter
	cv::Mat unsharpMaskBox(sourceImage.size(), sourceImage.type());
	cv::Mat unsharpMaskGauss(sourceImage.size(), sourceImage.type());
	cv::Mat unsharpMaskDiff(sourceImage.size(), sourceImage.type());

	filters.unsharpMask([&]() { cv::blur(sourceImage, unsharpMaskBox, {3, 3}); }, sourceImage, unsharpMaskBox, 1.5);
	filters.unsharpMask(
		[&]() { cv::GaussianBlur(sourceImage, unsharpMaskGauss, {3, 3}, 2); }, sourceImage, unsharpMaskGauss, 1.5);

	cv::absdiff(unsharpMaskBox, unsharpMaskGauss, unsharpMaskDiff);

	cv::imshow("unsharpMaskBox", unsharpMaskBox);
	cv::imshow("unsharpMaskGauss", unsharpMaskGauss);
	cv::imshow("unsharpMaskDiff", unsharpMaskDiff);

	if (cv::waitKey() == Defines::kQuitKey) {
		cv::destroyAllWindows();
	}

	// Paragraph 6: laplacian filter
	cv::Mat laplacianImage(sourceImage.size(), sourceImage.type());

	filters.laplacianFilter(sourceImage, laplacianImage);

	cv::imshow("laplacianFilter", laplacianImage);

	if (cv::waitKey() == Defines::kQuitKey) {
		cv::destroyAllWindows();
	}

	// Paragraph 7: unsharp mask for laplacian filter
	cv::Mat unsharpMaskLaplacian(sourceImage.size(), sourceImage.type());

	filters.unsharpMask(
		[&]() { filters.laplacianFilter(sourceImage, unsharpMaskLaplacian); }, sourceImage, unsharpMaskLaplacian, 1.5);

	cv::imshow("unsharpMaskLaplacian", unsharpMaskLaplacian);

	if (cv::waitKey() == Defines::kQuitKey) {
		cv::destroyAllWindows();
	}

	// Saving all images
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "customBoxFilter.png", customBoxFIlterImage);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "cvBlur.png", cvBlurImage);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "blurDiff.png", blurDiff);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "gaussianBlur.png", gaussianBlur);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "gaussDiff.png", gaussDiff);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "gaussLogDiff.png", gaussLogDiff);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "unsharpMaskBox.png", unsharpMaskBox);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "unsharpMaskGauss.png", unsharpMaskGauss);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "unsharpMaskDiff.png", unsharpMaskDiff);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "laplacianFilter.png", laplacianImage);
	cv::imwrite(Defines::resourcesPath / "Lab2Results" / "unsharpMaskLaplacian.png", unsharpMaskLaplacian);


	return 0;
}