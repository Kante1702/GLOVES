#pragma once
#ifndef GESTURELIBRARY_HPP
#define GESTURELIBRARY_HPP

#include "Gesture.hpp"
#include <vector>

class GestureLibrary {

public:
	GestureLibrary() {
		//thumb, index, middle, ring, pinky
		gestures.push_back(Gesture("Fist", { 0.0f,0.0f,0.0f ,0.0f ,0.0f /*0.1fqq*/}, {0.2f,0.2f ,0.2f ,0.2f ,0.8f}));

		gestures.push_back(Gesture("Point", {0.0f,0.55f ,0.0f ,0.0f,0.0f }, { 0.55f ,1.0f ,0.55f ,0.55f,0.55f }));
		
		//gestures.push_back(Gesture("ThumbsUp", {}, {})); //dokoncit

		//gestures.push_back(Gesture("FlatHand", {0.9f,0.9f,0.9f,0.9f,0.9f }, {1.0f,1.0f ,1.0f ,1.0f ,1.0f })); //dokoncit



		/////////////////////////////////////////////////////////////////////////////

		//prava ruka lock/unlock gesto
		gestures.push_back(Gesture("Lock_Unlock", {}, {},1000)); //thumb, index, little straight

		//prava TRANSLATION MODE - X,Y,Z
		gestures.push_back(Gesture("Right_X",{},{})); //X //index and middle straight

		gestures.push_back(Gesture("Right_Y", {}, {}));//Y //index,middle and ring straight

		gestures.push_back(Gesture("Right_Z", {}, {})); //Z //thumb closed others straights

		//prava ROTATION MODE - Yaw ,Pitch, Roll 
		gestures.push_back(Gesture("Right_Yaw", {}, {})); // Yaw //index straight

		gestures.push_back(Gesture("Right_Pitch", {}, {})); //Pitch //middle straight 

		gestures.push_back(Gesture("Right_Roll", {}, {})); //Roll //pinky straight

		//prava CUSTOM MODE - nieco 

		gestures.push_back(Gesture("Right_Custom_", {}, {})); //nieco //thumb and index straight

		//Prava mozno stop /start
		gestures.push_back(Gesture("Right_Start_Resume", {}, {})); //thumbs up
		gestures.push_back(Gesture("Right_Stop", { 0.0f,0.0f,0.0f ,0.0f ,0.0f }, { 0.2f,0.2f ,0.2f ,0.2f ,0.8f }));	//fist


		///// LAVA RUKA

		//xyz mode
		gestures.push_back(Gesture("Left_XYZ_Mode", {}, {})); //thumb and index straight 

		//Rotation mode
		gestures.push_back(Gesture("Left_Rotation_Mode", {}, {})); //thumb,index, midlle straight

		//Custom mode
		gestures.push_back(Gesture("Left_Custom_Mode", {}, {})); //thumb,index,middle,ring straight

		//Service mode
		gestures.push_back(Gesture("Left_Service_Mode", {}, {}));  //thumb and middle straight

		//kladny smer
		gestures.push_back(Gesture("Left_positive_direc", {}, {})); // index straight

		//zaporny smer
		gestures.push_back(Gesture("Left_negative_direc", {}, {})); //pinky straight

		//start/resuume 
		gestures.push_back(Gesture("Left_Start_Resume", {}, {})); // thumbs up

		//stop 
		gestures.push_back(Gesture("Left_stop",  {0.0f,0.0f,0.0f ,0.0f ,0.0f }, {0.2f,0.2f ,0.2f ,0.2f ,0.8f})); //fist




		/*
		
						DOKONCIT GESTAAA

		*/



	}
	const std::vector<Gesture>& getGesture() {
		return gestures;
	}


private:
	std::vector<Gesture> gestures;

};



#endif // !GESTURELIBRARY_HPP
