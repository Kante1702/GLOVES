// GestureExperiment.hpp
// Riadi priebeh experimentalnej verifikacie systemu rozpoznavania gest.
// Generuje nahodnu sekvenciu gest (lava a prava ruka sa striedaju), zobrazuje
// vyzvy na obrazovke a meria reakcny cas od zobrazenia vyzvy po detekciu spravneho
// gesta. Vysledky su priebezne zapisovane do CSV suborov.
//
// Striedanie ruk v sekvencii znizuje pocet faloshnych zaznamov v matici zamen,
// pretoze gesta z neocakavanej ruky su uplne ignorovane pri spracovani.
//
// Vystupne subory:
//   ExperimentResults_HHMMSS.csv  - reakcne casy pre kazde rozpoznane gesto
//   ExperimentAverages_HHMMSS.csv - priemerny reakcny cas a pocet merani na gesto
//   ConfusionMatrix_HHMMSS.csv    - matica zamen (ocakavane vs rozoznane gesto)


#pragma once
#ifndef GESTUREEXPERIMENT_HPP
#define GESTUREEXPERIMENT_HPP

#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <random>
#include <utility>
#include <unordered_map>

class GestureExperiment {

    bool m_waitingForStart = true;
    bool m_running = false;
    bool m_gestureAccepted = false; //blokuje zapisy do matice po spravnom rozoznani

    size_t m_currentIndex = 0;
    int m_repeatCount = 5;                  // pocet opakovani kazdeho gesta v sekvencii
    int m_delayBetweenGesturesMs = 1800;    // pauza medzi gestami - nezapocitava sa do reakcneho casu

    std::vector<std::pair<std::string, std::string>> m_gestures; // vstupny zoznam gest {ruka, nazov}
    std::vector<std::pair<std::string, std::string>> m_sequence; // vygenerovana nahodna sekvencia

    // m_confusionMatrix[ocakavane][rozoznane] = pocet
    std::unordered_map<std::string, std::unordered_map<std::string, int>> m_confusionMatrix; 

    std::unordered_map<std::string, long long> m_results;       // sucet reakcnych casov pre kazde gesto
    std::unordered_map<std::string, int> m_resultCounts;        // pocet uspesnych detekcii

    std::chrono::steady_clock::time_point m_startTime;          // cas zobrazenia aktualnej vyzvy
    std::ofstream m_file;

    // Generuje nahodnu sekvenciu so striedanim lavej a pravej ruky
    void buildRandomSequence();

    // Zobrazi vyzvu pre nasledujuce gesto v sekvencii
    void showNextGesture();

    void saveAverages();
    void saveConfusionMatrix();

public: 

    void start();       // spusti experiment s odpoitavanim a generovanim sekvencie
    void stopEarly();   // predcasne zastavi experiment a ulozi dostupne vysledky

    // Spracuje prichadzajuce gesto pocas experimentu.
    // Gesta z neocakavanej ruky su ignorovane.
    // Pri spravnom geste zaznamena cas a posunie sa na dalsi prvok sekvencie.
    // Pri nespravnom geste zaznamena zamenu do matice a caka na spravne gesto.
	void processGesture(const std::string& hand, const std::string& gesture);

	void setGestures(const std::vector<std::pair<std::string, std::string>>& gestures);
	bool isRunning() const { return m_running; }
};




#endif // !GESTUREEXPERIMENT_HPP

