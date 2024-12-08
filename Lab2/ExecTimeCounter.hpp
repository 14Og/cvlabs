#ifndef LAB2_EXECTIMECOUNTER
#define LAB2_EXECTIMECOUNTER

#include <chrono>
#include <functional>
#include <iostream>

namespace Lab2 {

struct ExecTimeCounter {
	template<typename Lambda>
	ExecTimeCounter(Lambda &&aLambda, const std::string &aFunctionName)
	{
		auto start = std::chrono::high_resolution_clock::now();
		aLambda();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		std::cout << aFunctionName << "() execution time: " << duration.count() << " microseconds" << std::endl;
	}
	ExecTimeCounter(const ExecTimeCounter &)			= delete;
	ExecTimeCounter &operator=(const ExecTimeCounter &) = delete;

};

}

#endif /* LAB2_EXECTIMECOUNTER */
