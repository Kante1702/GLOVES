#pragma once
#ifndef GESTURE_HPP
#define GESTURE_HPP

#include <array>
#include <string>
#include "HandType.hpp"

struct Gesture {
	std::string name; //napr fist..
	std::array<float, 5> lowerTresholds;
	std::array<float, 5> upperTresholds;
	int holdTimeMs;
	HandType hand;
	bool isGlobal; //  lock/unlock
	

	//kazde gesto je viazane na konkretnu ruku/rukavicu
	Gesture(const std::string& NAME, const std::array<float,5>& lowerTr , 
		const std::array<float,5>& upperTr , HandType h , int holdMs = 1, bool global = false)
		:name(NAME), lowerTresholds(lowerTr) , upperTresholds(upperTr),
		holdTimeMs(holdMs), hand(h) , isGlobal(global) {}



};

#endif // !GESTURE_HPP


