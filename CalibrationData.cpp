#include "CalibrationData.hpp"



CalibrationData::CalibrationData() {
	//inicializacia min max na extremy
	minValues.fill(99999.0f);
	maxValues.fill(-999999.0f);
}

//ukladanie raw pocas kalibracie 
void CalibrationData::update(const std::array<float, 5>& raw) {
	for (size_t i = 0; i < 5; i++) {
		minValues[i] = (std::min)(minValues[i], raw[i]);
		maxValues[i] = (std::max)(maxValues[i], raw[i]);
	}
}

//normalizacia

float CalibrationData::normalizeFinger(int finger, float raw) const
{
	float min = fist[finger];
	float max = open[finger];

	if (max - min < 0.001f)
		return 0.0f;

	float norma = (raw - min) / (max - min);
	norma = std::clamp(norma, 0.0f, 1.0f);

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