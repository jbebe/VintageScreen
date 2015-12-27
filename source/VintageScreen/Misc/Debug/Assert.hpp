#pragma once

#include <iostream>
#include <string>

#ifndef __TO_STR
	#define __TO_STR2(expr) #expr
	#define __TO_STR(expr) __TO_STR2(expr)
#endif

#define RT_ASSERT(condition, message) RuntimeAssert((condition), (message), __TO_STR(__FILE__), __TO_STR(__LINE__))
#define CT_ASSERT(condition, message) static_assert((condition), message)

void RuntimeAssert(bool condition, std::string message, std::string &&file, std::string &&line){
	if (condition == false){
		std::cout
			<< file << "(" << line << "): error:" << std::endl 
			<< "\t" << message << std::endl
			<< "press enter to exit..." << std::endl;
		std::cin.ignore(1);
		std::flush(std::cout);
		exit(1);
	}
}
