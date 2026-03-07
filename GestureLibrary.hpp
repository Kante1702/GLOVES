#pragma once
#ifndef GESTURELIBRARY_HPP
#define GESTURELIBRARY_HPP

#include "Gesture.hpp"
#include "HandType.hpp"
#include <vector>

class GestureLibrary {

public:
	GestureLibrary() {
		//thumb, index, middle, ring, pinky
		//gestures.push_back(Gesture("Fist", { 0.0f,0.0f,0.0f ,0.0f ,0.0f /*0.1fqq*/}, {0.2f,0.2f ,0.2f ,0.2f ,0.8f},HandType::Left));

		//gestures.push_back(Gesture("Point", {0.0f,0.55f ,0.0f ,0.0f,0.0f }, { 0.55f ,1.0f ,0.55f ,0.55f,0.55f })); 
		
		


		/////////////////////////////////////////////////////////////////////////////

		//prava ruka lock/unlock gesto
		gestures.push_back(Gesture("Lock_Unlock", { 0.6f,0.6f ,0.0f ,0.0f,0.6f }, { 1.0f ,1.0f ,0.4f ,0.4f,1.0f },HandType::Right,800, true)); //thumb, index, little straight

		//prava TRANSLATION MODE - X,Y,Z
		gestures.push_back(Gesture("Right_X",{ 0.0f,0.7f ,0.7f ,0.0f,0.0f},{0.4f ,1.0f ,1.0f ,0.4f,0.4f},HandType::Right)); //X //index and middle straight

		gestures.push_back(Gesture("Right_Y", { 0.0f,0.7f ,0.7f ,0.7f,0.0f }, { 0.4 ,1.0f ,1.0f ,1.0f,0.4f },HandType::Right));//Y //index,middle and ring straight

		gestures.push_back(Gesture("Right_Z", { 0.0f,0.8f ,0.8f ,0.8f,0.8f }, { 0.1f ,1.0f ,1.0f ,1.0f,1.0f },HandType::Right)); //Z //thumb closed others straights

		//prava ROTATION MODE - Yaw ,Pitch, Roll 
		gestures.push_back(Gesture("Right_Yaw", { 0.0f,0.65f ,0.0f ,0.0f,0.0f }, { 0.5f ,1.0f ,0.4f ,0.4f,0.4f }, HandType::Right)); // Yaw //index straight

		gestures.push_back(Gesture("Right_Pitch", { 0.0f,0.0f ,0.6f ,0.0f,0.0f }, { 0.4f ,0.4f ,1.0f ,0.45f,0.4f }, HandType::Right)); //Pitch //middle straight 

		gestures.push_back(Gesture("Right_Roll",{ 0.0f,0.0f ,0.0f ,0.0f,0.8f }, { 0.3f ,0.4f ,0.3f ,0.3f,1.0f }, HandType::Right)); //Roll //pinky straight

		//prava CUSTOM MODE 

		gestures.push_back(Gesture("Right_Custom_Mode", { 0.0f,0.0f ,0.6f ,0.6f,0.0f }, { 0.35f ,0.35f ,1.0f ,1.0f,0.35f }, HandType::Right,500)); //nieco //middle and ring straight

		gestures.push_back(Gesture("Right_RobotDisconnect", { 0.65f,0.0f ,0.0f ,0.0f,0.0f }, { 1.0f ,0.35f ,0.35f ,0.35f,0.35f }, HandType::Right, 500)); // Disconnect robot // Thumb straight 

		//Prava mozno stop /start
		gestures.push_back(Gesture("Right_Stop_Resume", { 0.0f,0.0f ,0.0f ,0.0f,0.0f }, { 0.4f,0.4f ,0.4f ,0.4f ,0.4f },HandType::Right,500)); //fist


	



		///// LAVA RUKA

		//xyz mode
		gestures.push_back(Gesture("Left_XYZ_Mode", {0.7f,0.7f ,0.0f ,0.0f,0.0f }, { 1.0f ,1.0f ,0.45f ,0.45f,0.45f },HandType::Left,750)); //thumb and index straight 

		//Rotation mode
		gestures.push_back(Gesture("Left_Rotation_Mode", { 0.7f,0.7f ,0.7f ,0.0f,0.0f }, { 1.0f ,1.0f ,1.0f ,0.45f,0.45f }, HandType::Left,750)); //thumb,index, midlle straight

		//Custom mode
		gestures.push_back(Gesture("Left_Custom_Mode", { 0.7f,0.7f ,0.7f ,0.65f,0.0f }, { 1.0f ,1.0f ,1.0f ,1.0f,0.45f }, HandType::Left,750)); //thumb,index,middle,ring straight

		//Service mode
		gestures.push_back(Gesture("Left_Service_Mode", { 0.7f,0.0f ,0.7f ,0.0f,0.0f }, { 1.0f ,0.45f ,1.0f ,0.45f,0.45f }, HandType::Left,750));  //thumb and middle straight
		
		//kladny smer
		gestures.push_back(Gesture("Left_Positive_Direction", { 0.0f,0.70f ,0.0f ,0.0f,0.0f }, { 0.4f ,1.0f ,0.4f ,0.4f,0.4f }, HandType::Left,500)); // index straight

		//zaporny smer
		gestures.push_back(Gesture("Left_Negative_Direction", { 0.0f,0.0f ,0.0f ,0.0f,0.7f }, { 0.4f ,0.4f ,0.4f ,0.65f,1.0f }, HandType::Left, 500)); //pinky straight

		//start/resuume 
		gestures.push_back(Gesture("Left_Stop_Resume", { 0.0f,0.0f ,0.0f ,0.0f,0.0f }, { 0.4f,0.4f ,0.4f ,0.4f ,0.4f },HandType::Left,500)); // fist

		


		// ============================================================== Testovacie gesta na experiment =============================================================
		gestures.push_back(Gesture("Gesto_1", { 0.0f,0.6f ,0.0f ,0.0f,0.6f }, { 0.4f ,1.0f ,0.4f ,0.4f,1.0f }, HandType::Right)); //index and pinky straight 
		
		gestures.push_back(Gesture("Gesto_2", {0.0f,0.0f, 0.6f,0.6f, 0.6f}, {0.4f,0.4f, 1.0f,1.0f,1.0f}, HandType::Right));

		gestures.push_back(Gesture("Gesto_3", {0.6f, 0.0f,0.0f, 0.0f, 0.6f}, {1.0f, 0.4f,0.4f, 0.4f , 1.0f}, HandType::Right));

		gestures.push_back(Gesture("Gesto_4", {0.0f,0.0f,0.6f,0.0f,0.6f}, {0.4f,0.4f,1.0f,0.4f,1.0f}, HandType::Left));

		gestures.push_back(Gesture("Gesto_5", {0.6f,0.6f, 0.6f, 0.0f, 0.6f}, {1.0f ,1.0f, 1.0f,0.4f,1.0f}, HandType::Left));

		gestures.push_back(Gesture("Gesto_6", {0.6f , 0.0f, 0.6f ,0.0f , 0.6f}, {1.0f , 0.4f, 1.0f ,0.4f, 1.0f}, HandType::Left));

	}

	std::vector<Gesture> getGesturesForHand(HandType hand) const {

		std::vector<Gesture> result;
		for (const auto& g : gestures) {
			if (g.hand == hand) {
				result.push_back(g);
			}

		}
		return result;
	}





private:
	std::vector<Gesture> gestures;

};



#endif // !GESTURELIBRARY_HPP
