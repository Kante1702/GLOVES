// Gesture.hpp
// Datova struktura reprezentujuca jedno rozpoznatelne gesto rukavice.
// Kazde gesto je definovane dolnymi a hornymi prahovymi hodnotami pre kazdy prst.
// Rozpoznanie prebieha porovnanim normalizovanych hodnot (0.0 = ohnuty, 1.0 = natiahnuTy)
// s tymito prahovymi hodnotami. Gesto je potvrdene az po tom, co su prahy splnene
// nepretrzite po dobu holdTimeMs milisekund.
//
// Poradie prstov v poliach: [palec, ukazovak, prostredak, prstenak, maly]



#pragma once
#ifndef GESTURE_HPP
#define GESTURE_HPP

#include <array>
#include <string>
#include "HandType.hpp"

struct Gesture {
	std::string name; 
	std::array<float, 5> lowerTresholds;	// dolne prahy prstov
	std::array<float, 5> upperTresholds;	// horne prahy prstov
	int holdTimeMs;		// minimalna doba drzania gesta pred potvrdenim
	HandType hand;		// priradenie k ruke
	bool isGlobal;		// ak true, gesto je aktivne bez ohladu na aktualny rezim (napr. Lock_Unlock)
	

	Gesture(const std::string& NAME, const std::array<float,5>& lowerTr , 
		const std::array<float,5>& upperTr , HandType h , int holdMs = 1, bool global = false)
		:name(NAME), lowerTresholds(lowerTr) , upperTresholds(upperTr),
		holdTimeMs(holdMs), hand(h) , isGlobal(global) {}

};

#endif // !GESTURE_HPP


