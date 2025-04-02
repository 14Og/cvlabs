#ifndef _LAB5_MAKEPHOTO_HPP_
#define _LAB5_MAKEPHOTO_HPP_

#include "CoreHeaders.hpp"
#include "Defines.hpp"

#include <iostream>
#include <string>

int main() {
    auto cap = cv::VideoCapture(0);
    auto path = Defines::resourcesPath / "Lab5" / "Matlab";
    int frameId = 0;
    while (cap.isOpened()) {
        cv::Mat image;
        cap.read(image);

        cv::imshow("Make photos helper", image);
        auto  key = cv::waitKey(10);

        if (key == Defines::kMakePhotoKey) {
            auto filename = path / std::to_string(++frameId);
            filename.replace_extension("jpg");
            cv::imwrite(filename, image);
            
            std::cout << "Saved photo # " << frameId << " into " << filename << std::endl;
        } else if (key == Defines::kQuitKey)
            break;

    }
    cv::destroyAllWindows();
    cap.release();
}

#endif /* _LAB5_MAKEPHOTO_HPP_ */
