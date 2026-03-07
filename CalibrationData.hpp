#pragma once
#ifndef CALIBRATIONDATA_HPP
#define CALIBRATIONDATA_HPP
#include <array>
#include <algorithm>


struct CalibrationData {

	std::array<float, 5> fist{};
	std::array<float, 5> open{};
	std::array<float, 5> maxValues;
	std::array<float, 5> minValues;

	bool openedSet = false;
	bool fistSet = false;
	

	CalibrationData();

	//ukladanie raw pocas kalibracie 
	void update(const std::array<float, 5>& raw) ;

	//normalizacia 
	float normalizeFinger(int finger, float raw) const;


	std::array<float, 5> normalizeAll(const std::array<float, 5>& raw) const;

};
#endif // !CALIBRATIONDATA_HPP
