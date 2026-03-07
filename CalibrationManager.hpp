
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
	std::unordered_map<std::string, CalibrationData> data;
	bool m_isCalibrating = false;
	std::unordered_map<std::string, std::vector<std::array<float, 5>>> m_openSamples;
	std::unordered_map<std::string, std::vector<std::array<float, 5>>> m_fistSamples;

	size_t m_requiredSamples = 100;

	void startCalibration();
	void finishCalibration();

	//raw sluzi na min/max
	void updateRaw(const std::string& gloveName, const std::array<float, 5>& raw);


	//uklada hodnoty pre otvorenu ruku (kalibracia)
	void setOpenHand(const std::string& gloveName, const std::array<float, 5>& values);
	

	//uklada hodnoty pre zatvorenu ruku (kalibracia)
	void setFist(const std::string& gloveName, const std::array<float, 5>& values);

	bool collectOpenSamples(const std::string& gloveName, const std::array<float, 5>& values);
	
	bool collectFistSamples(const std::string& gloveName, const std::array<float, 5>& values);
	


	//kotrola ci je kalibracia kompletna
	bool isCalibrated(const std::string& gloveName) const;

	//normalizacia
	std::array<float, 5> normalize(const std::string& gloveName, const std::array<float, 5>& raw) const;

	void saveToCSV(const std::string& gloveName);

	//nacitanie kalibracnych hodnot
	void loadFromCSV(const std::string& gloveName);
};



#endif // !CALIBRATIONMANAGER_HPP
