#include "CalibrationManager.hpp"




	void CalibrationManager::startCalibration() {
		m_isCalibrating = true;
		for (auto& [key, d] : data) {
			d = CalibrationData(); //reset;
		}
	}
	void CalibrationManager::finishCalibration() {
		m_isCalibrating = false;
	}

	//raw sluzi na min/max
	void CalibrationManager::updateRaw(const std::string& gloveName, const std::array<float, 5>& raw) {
		data[gloveName].update(raw);
	}


	//uklada hodnoty pre otvorenu ruku (kalibracia)
	void CalibrationManager::setOpenHand(const std::string& gloveName,
		const std::array<float, 5>& values)
	{
		auto& d = data[gloveName];

		for (int i = 0; i < 5; i++) {
			d.open[i] = values[i];
		}

		d.openedSet = true;
	}

	//uklada hodnoty pre zatvorenu ruku (kalibracia)
	void CalibrationManager::setFist(const std::string& gloveName, const std::array<float, 5>& values) {
		auto& d = data[gloveName];

		for (int i = 0; i < 5; i++) {
			d.fist[i] = values[i];
		}
		d.fistSet = true;
		// FIST = minimum (0.0)

	}
	bool CalibrationManager::collectOpenSamples(const std::string& gloveName,
		const std::array<float, 5>& values)
	{
		auto& samples = m_openSamples[gloveName];
		samples.push_back(values);

		if (samples.size() < m_requiredSamples)
			return false;

		auto& d = data[gloveName];

		for (int i = 0; i < 5; i++) {
			float sum = 0.0f;
			for (auto& s : samples)
				sum += s[i];

			d.open[i] = sum / samples.size();
		}

		d.openedSet = true;
		samples.clear();
		return true;
	}

	bool CalibrationManager::collectFistSamples(const std::string& gloveName,
		const std::array<float, 5>& values)
	{
		auto& samples = m_fistSamples[gloveName];
		samples.push_back(values);

		if (samples.size() < m_requiredSamples)
			return false;

		auto& d = data[gloveName];

		for (int i = 0; i < 5; i++) {
			float sum = 0.0f;
			for (auto& s : samples)
				sum += s[i];

			d.fist[i] = sum / samples.size();
		}

		d.fistSet = true;
		samples.clear();
		return true;
	}


	//kotrola ci je kalibracia kompletna
	bool CalibrationManager::isCalibrated(const std::string& gloveName) const {

		auto it = data.find(gloveName);
		if (it == data.end()) {
			return false;
		}
		return it->second.openedSet && it->second.fistSet;

	}

	//normalizacia
	std::array<float, 5> CalibrationManager::normalize(const std::string& gloveName, const std::array<float, 5>& raw) const {

		auto it = data.find(gloveName);
		if (it == data.end()) {
			return raw; //fallback
		}

		return it->second.normalizeAll(raw);
	}

	void CalibrationManager::saveToCSV(const std::string& gloveName) {

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

	void CalibrationManager::loadFromCSV(const std::string& gloveName) {


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



