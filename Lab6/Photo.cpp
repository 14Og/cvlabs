#ifndef _LAB6_MAIN_CPP_
#define _LAB6_MAIN_CPP_


#include "Defines.hpp"

#include "PhotoLocator.hpp"


int main()
{
    auto photo = Defines::resourcesPath / "Lab6" / "Photo" / "calib.jpg";
    PhotoLocator locator;
    locator.locate(cv::imread(photo.string()));
    
    while (cv::waitKey() != Defines::kQuitKey) {}
    cv::destroyAllWindows();
}


#endif /* _LAB6_MAIN_CPP_ */
