#ifndef LAB2_EXECTIMECOUNTER
#define LAB2_EXECTIMECOUNTER

#include <chrono>
#include <functional>
#include <iostream>

namespace Lab2 {


	void execTimeCounter(auto &&aLambda, const std::string &aFunctionName)
	{
		auto start = std::chrono::high_resolution_clock::now();
		aLambda();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		std::cout << aFunctionName << "() execution time: " << duration.count() << " microseconds" << std::endl;
	}


}

#endif /* LAB2_EXECTIMECOUNTER */
