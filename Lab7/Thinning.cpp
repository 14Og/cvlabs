#include "ThinningAlgo.hpp"
#include "Defines.hpp"

#include <vector>

void thinPhotos()
{
	auto photoPath  = Defines::resourcesPath / "Lab7" / "Photo" / "Thinning";

	auto letterImage = cv::imread(photoPath / "2.jpg", cv::IMREAD_GRAYSCALE);
    auto csImage = cv::imread(photoPath / "1.jpg", cv::IMREAD_GRAYSCALE);


    Thinning::performFull(letterImage);
    Thinning::performFull(csImage);
    cv::imshow("thinned letter", letterImage);
    cv::imshow("thinned cs", csImage);

    cv::waitKey();

    cv::destroyAllWindows();
}


int main()
{
    thinPhotos();

}