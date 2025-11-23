#pragma once
#ifndef GESTURELIBRARY_HPP
#define GESTURELIBRARY_HPP

#include "Gesture.hpp"
#include <vector>

class GestureLibrary {

public:
	GestureLibrary() {
		//thumb, index, middle, ring, pinky
		gestures.push_back(Gesture("Fist", { 0.0f,0.0f,0.0f ,0.0f ,0.0f /*0.1fqq*/}, {0.2f,0.2f ,0.2f ,0.2f ,0.4f}));

		gestures.push_back(Gesture("Point", {0.0f,0.55f ,0.0f ,0.0f,0.0f }, { 0.55f ,1.0f ,0.55f ,0.55f,0.55f }));

		//gestures.push_back(Gesture("ThumbsUp", {}, {})); //dokoncit

		gestures.push_back(Gesture("FlatHand", {0.9f,0.9f,0.9f,0.9f,0.9f }, {1.0f,1.0f ,1.0f ,1.0f ,1.0f })); //dokoncit

	}
	const std::vector<Gesture>& getGesture() {
		return gestures;
	}


private:
	std::vector<Gesture> gestures;

};



#endif // !GESTURELIBRARY_HPP
