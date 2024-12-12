#ifndef LAB3_MAIN
#define LAB3_MAIN

#include "Task1.hpp"
#include "Task2.hpp"
#include "Task3.hpp"
#include "Task4.hpp"

int main(int, char **, char **)
{
    Lab3::WarmPlacesFinder task1(Defines::resourcesPath / "Lab3" / "Task1");
    Lab3::EngineCompartmentFinder task2(Defines::resourcesPath / "Lab3" / "Task2");
    Lab3::RobotsFinder task3(Defines::resourcesPath / "Lab3" / "Task3");
    Lab3::WrenchChecker task4(Defines::resourcesPath / "Lab3" / "Task4");

    task1.findOnPhotos(Lab3::DisplayMode::SIMULTANEOUS);
    task1.findOnVideos(Lab3::DisplayMode::SUBSEQUENTIAL);

    task2.findOnPhotos(Lab3::DisplayMode::SUBSEQUENTIAL);
    
    task3.findOnPhotos(Lab3::DisplayMode::SUBSEQUENTIAL);
    task3.findOnVideos(Lab3::DisplayMode::SUBSEQUENTIAL);

    task4.ckeckContours();

}


#endif /* LAB3_MAIN */
