#include "body.hpp"
#include <array>
#include <cmath>
#include <iostream>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <vector>

static constexpr int WINDOW_SIZE = 800;
static constexpr int bodies_num = 3;

double scaleValue(double value, double in_min, double in_max, double out_min, double out_max)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int main()
{
    std::array<Body<bodies_num>, bodies_num> bodies = {
        Body<bodies_num>(1, cv::Point2d(400, 400), cv::Point2d(-1, -1), 1000),
        Body<bodies_num>(2, cv::Point2d(300, 400), cv::Point2d(4, 0), 100),
        Body<bodies_num>(3, cv::Point2d(400, 300), cv::Point2d(1, 1), 1000)
    };

    std::array<body_state_vector, bodies_num> final_arr = rk4_run<bodies_num>(0.1, 10000, bodies);

    cv::namedWindow("Three-Body Problem");

    int i = 0;

    while (true) {
        cv::Mat image(WINDOW_SIZE, WINDOW_SIZE, CV_8UC3, cv::Scalar(0, 0, 0));

        if (i >= 10000) {
            i = 0;
        }
        cv::circle(image, cv::Point2d(final_arr[0][i].x, final_arr[0][i].y), 5, cv::Scalar(255, 0, 0), -1);
        cv::circle(image, cv::Point2d(final_arr[1][i].x, final_arr[1][i].y), 5, cv::Scalar(0, 255, 0), -1);
        cv::circle(image, cv::Point2d(final_arr[2][i].x, final_arr[2][i].y), 5, cv::Scalar(0, 0, 255), -1);
        i += 1;

        // Отображаем изображение
        cv::imshow("Three-Body Problem", image);

        // Задержка для создания анимации
        if (cv::waitKey(30) == 27) { // Нажмите ESC для выхода
            break;
        }
    }

    return 0;
}