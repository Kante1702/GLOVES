// CalibrationManager.hpp
// Sprava kalibracie pre obe rukavice.
// Po stlaceni klavesy 's' sa zacnu zberat vzorky - prvy m_requiredSamples frameov
// od momentu potvrdenia su ulozene do docasneho vektora, z ktoreho sa nakoniec
// vypocita priemer pre kazdy prst. Tento priemer sa ulozi ako referencna hodnota
// (open alebo fist) do prislusneho CalibrationData objektu.
// Kalibracne data su ukladane do / nacitavane zo suborov calibration_Left.csv
// a calibration_Right.csv, takze kalibracia nie je potrebna pri kazdom spusteni.



#pragma once
#ifndef CALIBRATIONMANAGER_HPP
#define CALIBRATIONMANAGER_HPP
#include "CalibrationData.hpp"
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>



class CalibrationManager {

public:
	std::unordered_map<std::string, CalibrationData> data;			// kalibracne data indexovane nazvom ruky ("Left"/"Right")
	bool m_isCalibrating = false;
	std::unordered_map<std::string, std::vector<std::array<float, 5>>> m_openSamples;		// docasne vzorky otvorenej ruky
	std::unordered_map<std::string, std::vector<std::array<float, 5>>> m_fistSamples;		// docasne vzorky zatvorenej ruky

	size_t m_requiredSamples = 100;		// pocet vzoriek potrebnych na jeden krok kalibracie

	void startCalibration();			// resetuje data a nastavi priznak kalibracie
	void finishCalibration();			// ukonci kalibraciu

	void updateRaw(const std::string& gloveName, const std::array<float, 5>& raw);
	void setOpenHand(const std::string& gloveName, const std::array<float, 5>& values);
	void setFist(const std::string& gloveName, const std::array<float, 5>& values);

	// Zbiera prvych m_requiredSamples vzoriek, potom vypocita priemer a vrati true
	bool collectOpenSamples(const std::string& gloveName, const std::array<float, 5>& values);
	bool collectFistSamples(const std::string& gloveName, const std::array<float, 5>& values);
	

	// Vrati true ak su nastavene obe referencne polohy pre danu rukavicu
	bool isCalibrated(const std::string& gloveName) const;

	// Normalizuje surove hodnoty prstov; ak kalibracia chyba, vrati povodne hodnoty
	std::array<float, 5> normalize(const std::string& gloveName, const std::array<float, 5>& raw) const;

	void saveToCSV(const std::string& gloveName);
	void loadFromCSV(const std::string& gloveName);
};
#endif // !CALIBRATIONMANAGER_HPP
