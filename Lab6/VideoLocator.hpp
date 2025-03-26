#include "PhotoLocator.hpp"


class VideoLocaror: public PhotoLocator {


public:
    VideoLocaror()
    {

    }

    void locate(std::string aVideoSource)
    {
        cv::VideoCapture cap(std::move(aVideoSource));
        
        while (1)
        {
            cv::Mat image;
            cap >> image;
            if (image.empty())
                break;
            
            PhotoLocator::locate(std::move(image), true);
            if (cv::waitKey(30) == Defines::kQuitKey)
                break;
        }
        cap.release();
    }
};

