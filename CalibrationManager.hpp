
#pragma once
#ifndef CALIBRATIONMANAGER_HPP
#define CALIBRATIONMANAGER_HPP
#include "CalibrationData.hpp"
#include <unordered_map>
#include <string>





class CalibrationManager {

public:
	std::unordered_map<std::string, CalibrationData> data;
	bool m_isCalibrating = false;

	void startCalibration() {
		m_isCalibrating = true;
		for (auto& [key, d] : data) {
			d = CalibrationData(); //reset;
		}
	}
	void finishCalibration() {
		m_isCalibrating = false;
	}

	//raw sluzi na min/max
	void updateRaw(const std::string& gloveName, const std::array<float, 5>& raw) {
		data[gloveName].update(raw);
	}


	//uklada hodnoty pre otvorenu ruku (kalibracia)
	void setOpenHand(const std::string& gloveName,
		const std::array<float, 5>& values)
	{
		auto& d = data[gloveName];

		for (int i = 0; i < 5; i++) {
			d.open[i] = values[i];
		}

		d.openedSet = true;
	}

	//uklada hodnoty pre zatvorenu ruku (kalibracia)
	void setFist(const std::string& gloveName, const std::array<float, 5>& values) {
		auto& d = data[gloveName];

		for (int i = 0; i < 5; i++) {
			d.fist[i] = values[i];
		}
		d.fistSet = true;
		// FIST = minimum (0.0)

	}

	//kotrola ci je kalibracia kompletna
	bool isCalibrated(const std::string& gloveName) const {

		auto it = data.find(gloveName);
		if (it == data.end()) {
			return false;
		}
		return it->second.openedSet && it->second.fistSet;

	}

	//normalizacia
	std::array<float, 5> normalize(const std::string& gloveName, const std::array<float, 5>& raw) const {

		auto it = data.find(gloveName);
		if (it == data.end()) {
			return raw; //fallback
		}

		return it->second.normalizeAll(raw);
	}

	void saveToCSV(const std::string& gloveName) {

		auto it = data.find(gloveName);
		if (it == data.end()) return;

		const CalibrationData& d = it->second;

		std::ofstream file("calibration_" + gloveName + ".csv");
		if (!file.is_open()) return;

		// MIN
		file << "Min;";
		for (int i = 0; i < 5; i++) file << d.minValues[i] << (i < 4 ? ";" : "");
		file << "\n";

		// MAX
		file << "Max;";
		for (int i = 0; i < 5; i++) file << d.maxValues[i] << (i < 4 ? ";" : "");
		file << "\n";

		// OPEN HAND
		file << "Open;";
		for (int i = 0; i < 5; i++) file << d.open[i] << (i < 4 ? ";" : "");
		file << "\n";

		// FIST HAND
		file << "Fist;";
		for (int i = 0; i < 5; i++) file << d.fist[i] << (i < 4 ? ";" : "");
		file << "\n";

		file.close();
		std::cout << "[Calibration] Saved CSV for " << gloveName << std::endl;
	}

	void loadFromCSV(const std::string& gloveName) {


		std::ifstream file("calibration_" + gloveName + ".csv");
		if (!file.is_open()) {
			std::cout << "[Calibration] NO CSV for " << gloveName << ", skipping\n";
			return;

		}

		CalibrationData d;

		std::string line;
		while (std::getline(file, line)) {

			if (line.empty()) {
				continue;
			}

			std::stringstream ss(line);
			std::string tag;
			std::getline(ss, tag, ';');

			for (int i = 0; i < 5; i++) {
				std::string val;
				if (!std::getline(ss, val, ';')) break; //nedostatok columns na riadku

				try {
					float f = std::stof(val);

					if (tag == "Min")  d.minValues[i] = f;
					if (tag == "Max")  d.maxValues[i] = f;
					if (tag == "Open") { d.open[i] = f; d.openedSet = true; }
					if (tag == "Fist") { d.fist[i] = f; d.fistSet = true; }

				}
				catch (...) {
					std::cout << "[Calibration] Invalid float: " << val << "\n";
				}
			}
		}

		data[gloveName] = d;

		if (d.openedSet && d.fistSet) {
			m_isCalibrating = false;
		}
		std::cout << "[Calibration] Loaded CSV for " << gloveName << std::endl;
	}



};



#endif // !CALIBRATIONMANAGER_HPP
