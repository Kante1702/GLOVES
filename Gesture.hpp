#pragma once
#ifndef GESTURE_HPP
#define GESTURE_HPP

#include <array>
#include <string>

struct Gesture {
	std::string name; //napr fist..
	std::array<float, 5> lowerTresholds;
	std::array<float, 5> upperTresholds;
	int holdTimeMs;

	Gesture(const std::string& NAME, const std::array<float,5>& lowerTr , 
		const std::array<float,5>& upperTr , int holdMs = 250):name(NAME), lowerTresholds(lowerTr) , upperTresholds(upperTr){}



};

#endif // !GESTURE_HPP


