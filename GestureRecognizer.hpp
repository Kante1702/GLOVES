// GestureRecognizer.hpp
// Rozpoznavac gest v realnom case pre jednu rukavicu.
// Pri kazdom prichode BT dat sa pre kazde gesto v databaze skontroluje,
// ci su splnene prahy vsetkych 5 prstov. Ak ano, spusti sa casovy citac.
// Gesto je potvrdene az po tom, co su prahy splnene nepretrzite po dobu
// holdTimeMs milisekund. V experimentalnom rezime je tato doba skratena
// na 1 ms pre okamzitu detekciu bez cakania.
// Po kazdom potvrdenom geste je potrebne zavolat reset() pre vymazanie
// casovych citacov, aby sa predislo nasobnej detekcii.


#pragma once
#ifndef GESTURERECOGNIZER_HPP
#define GESTURERECOGNIZER_HPP
#include "GestureLibrary.hpp"
#include "HandType.hpp"
#include <optional>
#include <string>
#include <array>
#include <unordered_map>
#include <chrono>


class GestureRecognizer {

	GestureLibrary library;
	HandType hand;


	// Casova znacka zaciaku splnenia podmienok pre kazde gesto
	std::unordered_map<std::string, std::chrono::steady_clock::time_point> gestureStartTime;
	bool m_experimentMode = false;
	


public:
	//experiment
	GestureRecognizer(HandType h) : hand(h) {}

	// Vymaze vsetky priebezne casove citace - volat po kazdom potvrdenom geste
	void reset() {
		gestureStartTime.clear();
	}

	// Zapina experimentalny rezim, kde holdTimeMs = 1 ms pre okamzitu detekciu
	void setExperimentMode(bool val) {
		m_experimentMode = val; 
	};

	
	// Hlavna funkcia - volana pri kazdom BT frame.
	// Vrati nazov gesta ak bolo potvrdene, inak vrati nullopt.
	std::optional<std::string> recognize(const std::array<float, 5>& fingerValues ) {
	
		auto now = std::chrono::steady_clock::now();
		auto gestures = library.getGesturesForHand(hand);
		

		for (const auto& gesture : gestures) {
			bool matching = true;

			// Kontrola ci su splnene prahy pre vsetkych 5 prstov
			for (size_t i = 0; i < 5; ++i) {
				if (fingerValues[i] < gesture.lowerTresholds[i] || fingerValues[i] > gesture.upperTresholds[i]) {
					matching = false;
					break;
				}
			}

			if (matching) {

				if (!gestureStartTime.count(gesture.name) ){
					// Prva iteracia kde su prahy splnene - zaciname merat cas
					gestureStartTime[gesture.name] = now;
				}
				else {

					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - gestureStartTime[gesture.name]).count();
					int holdMs = m_experimentMode ? 1 : gesture.holdTimeMs;

					if (elapsed >= holdMs) {

						gestureStartTime.erase(gesture.name);		// reset citaca po detekcii
						return gesture.name;
					}
				}
			}
			else {
				gestureStartTime.erase(gesture.name);				 // Prahy prestali byt splnene - resetujeme citac pre toto gesto
			}

		}
		return std::nullopt;

	}

};



#endif // !GESTURERECOGNIZER_HPP
