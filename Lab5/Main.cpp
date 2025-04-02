#include "Aruco.hpp"

int main()
{
	auto calibParams = Defines::resourcesPath / "Lab5" / "calibration_params.yml";
	constexpr auto kArucoDictId = cv::aruco::DICT_4X4_50;
	
	[[maybe_unused]] constexpr auto kMarkersX = 4;
	[[maybe_unused]] constexpr auto kMarkersY = 5;
	[[maybe_unused]] constexpr auto kMarkerSize = 0.03f;
	[[maybe_unused]] constexpr auto kSeparatorSize = 0.01f;

	Aruco::calibrateCamera(calibParams, kMarkersX, kMarkersY, kMarkerSize, kSeparatorSize, kArucoDictId);
	Aruco::detectMarkers(calibParams, kMarkerSize, kArucoDictId);
	return 0;
}