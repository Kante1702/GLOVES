#pragma once
#ifndef GESTURELIBRARY_HPP
#define GESTURELIBRARY_HPP

#include "Gesture.hpp"
#include <vector>

class GestureLibrary {

public:
	GestureLibrary() {
		
		gestures.push_back(Gesture("Fist", { 0.0f,0.0f,0.0f ,0.0f ,0.0f }, { 1.0f,1.0f ,1.0f ,1.0f ,1.0f }));

		gestures.push_back(Gesture("Point", {3500.0f,3500.0f ,3500.0f ,3500.0f,3500.0f }, { 4500.0f ,4500.0f ,4500.0f ,4500.0f,4500.0f }));

	}
	const std::vector<Gesture>& getGesture() {
		return gestures;
	}


private:
	std::vector<Gesture> gestures;

};



#endif // !GESTURELIBRARY_HPP
