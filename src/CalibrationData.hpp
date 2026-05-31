// CalibrationData.hpp
// Ulozisko kalibracnych dat pre jednu rukavicu a logika normalizacie surovych hodnot.
// open[] obsahuje referencne hodnoty pri plne otvorenej ruke - mapuje sa na 1.0.
// fist[] obsahuje referencne hodnoty pri plne zatvorenej ruke (past) - mapuje sa na 0.0.
// Normalizovane hodnoty pod 0.3 su orezane na 0.0, aby sa potlacil sum senzorov
// v pokojovej polohe prstov.



#pragma once
#ifndef CALIBRATIONDATA_HPP
#define CALIBRATIONDATA_HPP
#include <array>
#include <algorithm>


struct CalibrationData {

	std::array<float, 5> fist{};			// surove hodnoty pri zatvorenej ruke
	std::array<float, 5> open{};			// surove hodnoty pri otvorenej ruke
	std::array<float, 5> maxValues;			// zaznamene maximum pocas kalibracie
	std::array<float, 5> minValues;			// zaznamene minimum pocas kalibracie

	bool openedSet = false;					// true = hodnoty otvorenej ruky su nastavene
	bool fistSet = false;					// true = hodnoty zatvorenej ruky su nastavene
	

	CalibrationData();

	// Aktualizuje sledovane min/max hodnoty pocas zberu kalibracnych vzoriek
	void update(const std::array<float, 5>& raw) ;

	// Normalizuje surovu hodnotu jedneho prsta do rozsahu [0.0, 1.0]
	float normalizeFinger(int finger, float raw) const;

	// Normalizuje vsetkych 5 prstov naraz
	std::array<float, 5> normalizeAll(const std::array<float, 5>& raw) const;

};
#endif // !CALIBRATIONDATA_HPP
