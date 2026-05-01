// CalibrationData.cpp
// Implementacia normalizacie senzorových hodnot prstov pre jednu rukavicu.


#include "CalibrationData.hpp"

CalibrationData::CalibrationData() {
	// Inicializacia na extremne hodnoty, aby prva vzorka spravne nastavila rozsah
	minValues.fill(99999.0f);
	maxValues.fill(-999999.0f);
}

void CalibrationData::update(const std::array<float, 5>& raw) {
	// Priebezna aktualizacia minima a maxima pocas zberu kalibracnych vzoriek
	for (size_t i = 0; i < 5; i++) {
		minValues[i] = (std::min)(minValues[i], raw[i]);
		maxValues[i] = (std::max)(maxValues[i], raw[i]);
	}
}


float CalibrationData::normalizeFinger(int finger, float raw) const
{
	float min = fist[finger];		// fist = referencia pre hodnotu 0.0
	float max = open[finger];		// open = referencia pre hodnotu 1.0

	// Ochrana pred delenim nulou, ak su obe referencie rovnake
	if (max - min < 0.001f)
		return 0.0f;
	
	float norma = (raw - min) / (max - min);
	
	// Orezanie na rozsah [0.0, 1.0] - eliminacia hodnot mimo kalibrovaneho rozsahu
	norma = std::clamp(norma, 0.0f, 1.0f);

	// Potlacenie sumu - hodnoty pod 0.3 su povazovane za pokojnu polohu
	if (norma < 0.3f)
		norma = 0.0f;

	return norma;

}

std::array<float, 5> CalibrationData::normalizeAll(const std::array<float, 5>& raw) const {
	std::array<float, 5> out{ };
	for (size_t i = 0; i < 5; i++) {
		out[i] = normalizeFinger(i, raw[i]);
	}
	return out;
}