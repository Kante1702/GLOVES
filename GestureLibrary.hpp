#pragma once
#ifndef GESTURELIBRARY_HPP
#define GESTURELIBRARY_HPP

#include "Gesture.hpp"
#include <vector>

class GestureLibrary {

public:
	GestureLibrary() {
		
		gestures.push_back(Gesture("Fist", { 1000.0f,3200.0f,2000.0f ,2100.0f ,1500.0f }, { 2000.0f,4000.0f ,3000.0f ,3000.0f ,2500.0f }));

		gestures.push_back(Gesture("Point", {3500.0f,3500.0f ,3500.0f ,3500.0f,3500.0f }, { 4500.0f ,4500.0f ,4500.0f ,4500.0f,4500.0f }));

	}
	const std::vector<Gesture>& getGesture() {
		return gestures;
	}


private:
	std::vector<Gesture> gestures;

};



#endif // !GESTURELIBRARY_HPP
