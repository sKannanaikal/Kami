#include <iostream>
#include "./lib/test.h"

int main() {
	std::cout << "Hello World!" << std::endl;
	std::cout << add(3, 2) << std::endl;
	std::cout << subtract(3, 2) << std::endl;
	std::cout << multiply(3, 2) << std::endl;
	std::cout << "Good Bye World!" << std::endl;
	return 0;
}
