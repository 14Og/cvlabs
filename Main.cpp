#include <iostream>
#include <vector>
#include "Lab1/Lab1.hpp"
#include "Lab1/ThreeBodyProblem.hpp"

int main(int, char **, char **)
{

	using ThreadContainer = std::vector<decltype(Lab1::VacuumCleanerAnimation::animate())>;
	ThreadContainer threads;

	// threads.push_back(Lab1::VacuumCleanerAnimation::animate());
    threads.push_back(Lab1::ThreeBodyProblem::animate());

	for (auto &thread : threads) {
		thread->join();
	}

	return 0;
}
