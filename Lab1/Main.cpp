#include "Lab1.hpp"
#include "ThreeBodyProblem.hpp"


int main(int, char **, char **)
{
    ThreeBP::RungeKutta4Runner::animate();
    Lab1::VacuumCleanerAnimation::animate();
}