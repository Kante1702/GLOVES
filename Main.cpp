//#include "GloveConnection.hpp"

#include "GloveValues.hpp"
#include <iostream>

int main() {

	 GloveValues glove;

	std::cout << "Hladam rukavicu... (stlac Enter pre ukoncenie)" << std::endl;

	if (!glove.connect()) {
		std::cout << "nepodarilo sa spustit central adapter" << std::endl;
		return -1;
	}

	

	// kym nestlacim enter
	std::cin.get();

	glove.disconnect();

	std::cout << "Aplikacia ukoncena." << std::endl;
	return 0;


}