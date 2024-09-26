#ifndef LAB1_LAB1
#define LAB1_LAB1

#include "../core.hpp"

namespace lab1 {
    void test()
    {
        auto img = cv::imread("/home/og/coding/study/cv/labs/resources/1.tif");
        img.resize(2);
    }
}

#endif /* LAB1_LAB1 */
