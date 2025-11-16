//#include "GloveConnection.hpp"

#include "ImuConfiguration.hpp"
#include <iostream>
#include <conio.h>

int main() {

	//GloveValues glove;// keby chcem len data zo senzoru 
	ImuConfiguration glove; //okrem dat mame aj imu config 



	glove.m_calibrationManager.loadFromCSV("Left");
	glove.m_calibrationManager.loadFromCSV("Right");
	glove.m_calibrationManager.finishCalibration();
	glove.setOnlyBendingLog(true);
	

	std::cout << "Hladam rukavicu..." << std::endl;

	if (!glove.connect()) {
		std::cout << "nepodarilo sa spustit central adapter" << std::endl;
		return -1;
	}

	std::cout << "\n=== OVLADANIE ===\n"
		"c  -> zacat kalibraciu\n"
		"s  -> ulozit krok\n"
		"q  -> koniec programu\n"
		"----------------------\n";
	
	// kym nestlacim enter
	std::string input;
	bool running = true;



	while (running) {


		if (_kbhit()) {

			char c = _getch();
			if (c == 'q' || c == 'Q') {
				running = false;
			}
			else if (c == 'c' || c == 'C') {
				glove.startCalibrating();
			}
			else if (c == 's' || c == 'S' ) {
				glove.confirmCalibrationStep(glove.m_lastRawLeft, glove.m_lastRawRight);
			}
		}
	}

	glove.disconnect();
	std::cout << "Aplikacia ukoncena." << std::endl;
	return 0;


}