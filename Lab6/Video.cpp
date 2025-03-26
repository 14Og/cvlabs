#include "Defines.hpp"


#include "VideoLocator.hpp"

int main()
{
    auto video = Defines::resourcesPath / "Lab6" / "Video" / "1.avi";
    VideoLocaror locator;
    locator.locate(video);

}