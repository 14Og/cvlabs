#ifndef LAB3_BASE
#define LAB3_BASE

#include "CoreHeaders.hpp"
#include "Defines.hpp"

#include <cstdint>
#include <filesystem>
#include <vector>
#include <ranges>
#include <iostream>

namespace Lab3 {

enum class DisplayMode : uint8_t { SIMULTANEOUS = 0, SUBSEQUENTIAL };


class FinderBase {

protected:
	static constexpr auto kPhotosExt{".jpg"};
	static constexpr auto kVideosExt{".mp4"};
	using Path = std::filesystem::path;
	using Iter = std::filesystem::directory_iterator;

public:
	FinderBase(const std::string aMediaPathname): mediaPath(std::move(aMediaPathname))
	{
		initFilenames();
	}

	virtual void findOnPhotos(DisplayMode) = 0;
	virtual void findOnVideos(DisplayMode)
	{
	}

private:
	void initFilenames()
	{
		std::ranges::for_each(Iter(mediaPath), [this](const auto &aDirEntry) {
			if (aDirEntry.path().extension() == kPhotosExt)
				absPhotoPathnames.push_back(aDirEntry.path().string());
			else if (aDirEntry.path().extension() == kVideosExt)
				absVideoPathnames.push_back(aDirEntry.path().string());
		});
	}


protected:
	Path mediaPath;
	std::vector<Path> absPhotoPathnames;
	std::vector<Path> absVideoPathnames;
};

class HSVThresholdHelper {
	using Pathnames = std::vector<std::filesystem::path>;

public:
	HSVThresholdHelper(const Pathnames &aPhotos)
	{
		for (const auto &photo : aPhotos) {
			auto image = cv::imread(photo.string(), cv::IMREAD_COLOR);
			cv::cvtColor(image, image, cv::COLOR_BGR2HSV);
			hsvImages.push_back(image);
			binarizedImages.push_back(image);
		}
	}

	void setupTrackBars()
	{
		cv::namedWindow("Control panel", cv::WINDOW_NORMAL);
		cv::createButton("dilate", onDilateFlag, this, cv::QT_CHECKBOX, false);
		cv::createButton("erode", onErodeFlag, this, cv::QT_CHECKBOX, false);
		cv::createTrackbar("h min", "Control panel", &hMin, 255, onThreshTrackbar, this);
		cv::createTrackbar("h max", "Control panel", &hMax, 255, onThreshTrackbar, this);
		cv::createTrackbar("s min", "Control panel", &sMin, 255, onThreshTrackbar, this);
		cv::createTrackbar("s max", "Control panel", &sMax, 255, onThreshTrackbar, this);
		cv::createTrackbar("v min", "Control panel", &vMin, 255, onThreshTrackbar, this);
		cv::createTrackbar("v max", "Control panel", &vMax, 255, onThreshTrackbar, this);
		cv::createTrackbar("kernel size", "Control panel", &kernelSize, 25, onKernelTrackbar, this);
	}

	void start()
	{
		setupTrackBars();
		updateImages();
		cv::setMouseCallback("HSV", onMouseHover, &hsvStack);
		if (cv::waitKey() == Defines::kQuitKey)
			cv::destroyAllWindows();
	}


private:
	static inline void onThreshTrackbar(int, void *aSelf)
	{
		auto self		  = reinterpret_cast<HSVThresholdHelper *>(aSelf);
		auto dilateKernel = cv::getStructuringElement(cv::MORPH_RECT, {self->kernelSize, self->kernelSize});
		auto erodeKernel  = cv::getStructuringElement(cv::MORPH_RECT, {self->kernelSize, self->kernelSize});

		for (size_t i = 0; i < self->hsvImages.size(); ++i) {
			self->binarizedImages.push_back(cv::Mat());
			cv::inRange(self->hsvImages[i], cv::Scalar(self->hMin, self->sMin, self->vMin), cv::Scalar(self->hMax, self->sMax, self->vMax),
						self->binarizedImages[i]);
			if (self->erodeFlag) {
				cv::erode(self->binarizedImages[i], self->binarizedImages[i], erodeKernel);
			}
			if (self->dilateFlag) {
				cv::dilate(self->binarizedImages[i], self->binarizedImages[i], dilateKernel);
			}
		}
		self->updateImages();
	}

	static inline void onKernelTrackbar(int aVal, void *aSelf)
	{
		auto self = reinterpret_cast<HSVThresholdHelper *>(aSelf);
		if (aVal % 2 == 0) {
			++aVal;
		}
		self->kernelSize = aVal;
		onThreshTrackbar(0, aSelf);
	}

	static inline void onDilateFlag(int aVal, void *aSelf)
	{
		auto self		 = reinterpret_cast<HSVThresholdHelper *>(aSelf);
		self->dilateFlag = static_cast<bool>(aVal);
	}

	static inline void onErodeFlag(int aVal, void *aSelf)
	{
		auto self		= reinterpret_cast<HSVThresholdHelper *>(aSelf);
		self->erodeFlag = static_cast<bool>(aVal);
	}

	void updateImages()
	{
		auto cellW = 640;
		auto cellH = 480;

		auto size  = hsvImages.size();
		hsvStack   = cv::Mat(cellH, cellW * size, hsvImages[0].type());
		binStack   = cv::Mat(cellH, cellW * size, binarizedImages[0].type());

		for (size_t i = 0; i < size; ++i) {
			cv::Mat resized;
			cv::resize(hsvImages[i], resized, {cellW, cellH});
			resized.copyTo(hsvStack(cv::Rect(i * cellW, 0, cellW, cellH)));
			cv::resize(binarizedImages[i], resized, {cellW, cellH});
			resized.copyTo(binStack(cv::Rect(i * cellW, 0, cellW, cellH)));
		}
		cv::imshow("HSV", hsvStack);

		cv::imshow("Binarized", binStack);
	}

	static inline void onMouseHover(int, int aX, int aY, int, void *aUserdata)
	{
		auto hsvImage = reinterpret_cast<cv::Mat *>(aUserdata);
		cv::Vec3b hsv = hsvImage->at<cv::Vec3b>(aY, aX);
		std::cout << "HSV: " << hsv << std::endl;
	}

private:

	int hMin{0};
	int hMax{255};
	int sMin{0};
	int sMax{255};
	int vMin{0};
	int vMax{255};

	bool dilateFlag{false};
	bool erodeFlag{false};

	int kernelSize{1};

	std::vector<cv::Mat> hsvImages;
	std::vector<cv::Mat> binarizedImages;

	cv::Mat hsvStack;
	cv::Mat binStack;
};

} // namespace Lab3

#endif /* LAB3_BASE */
