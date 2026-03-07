#include "ImuConfiguration.hpp"
#include <iostream>
#include <conio.h>
#include <thread> // dôležité pre sleep

int main() {
    // 1. Inicializácia
    ImuConfiguration glove;

    glove.m_calibrationManager.loadFromCSV("Left");
    glove.m_calibrationManager.loadFromCSV("Right");
    glove.m_calibrationManager.finishCalibration();
    glove.setOnlyBendingLog(true);

    std::cout << "Hladam rukavice a pripajam..." << std::endl;

    // 2. Pripojenie (štart skenovania a callbackov)
    if (!glove.connect()) {
        std::cout << "CHYBA: Nepodarilo sa spustit adapter!" << std::endl;
        return -1;
    }

    std::cout << "\n=== OVLADANIE ===\n"
        "c  -> zacat kalibraciu\n"
        "s  -> ulozit krok\n"
        "q  -> KONIEC PROGRAMU \n"
        "----------------------\n";

    bool running = true;
    while (running) {
        if (_kbhit()) {
            char c = _getch();
            if (c == 'q' || c == 'Q') {
                std::cout << "\nEnding... Disconnecting bluetooth streams..." << std::endl;

                
                glove.disconnect();

                
                std::this_thread::sleep_for(std::chrono::milliseconds(300));

                running = false;
            }
            else if (c == 'c' || c == 'C') {
                glove.startCalibrating();
            }
            else if (c == 's' || c == 'S') {
                glove.confirmCalibrationStep(glove.m_lastRawLeft, glove.m_lastRawRight);
            }
        }

        // Šetríme procesor
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Application succesfully ended" << std::endl;
    return 0;
}