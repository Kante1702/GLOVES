#pragma once
#ifndef GESTURERECOGNIZER_HPP
#define GESTURERECOGNIZER_HPP
#include "GestureLibrary.hpp"
#include "HandType.hpp"
#include <optional>
#include <string>
#include <array>
#include <unordered_map>;
#include <chrono>


class GestureRecognizer {

	GestureLibrary library;
	HandType hand;


	//uchovavanie start casu pre kazde gesto
	std::unordered_map<std::string, std::chrono::steady_clock::time_point> gestureStartTime;

	


public:
	GestureRecognizer(HandType h) : hand(h) {}

	std::optional<std::string> recognize(const std::array<float, 5>& fingerValues ) {
		

		auto now = std::chrono::steady_clock::now();
		auto gestures = library.getGesturesForHand(hand);

	


		for (const auto& gesture : gestures) {

			bool matching = true;

			for (size_t i = 0; i < 5; ++i) {
				if (fingerValues[i] < gesture.lowerTresholds[i] || fingerValues[i] > gesture.upperTresholds[i]) {
					matching = false;
					break;
				}
			}

			if (matching) {

				if (!gestureStartTime.count(gesture.name) ){
					gestureStartTime[gesture.name] = now;
				}
				else {

					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - gestureStartTime[gesture.name]).count();

					if (elapsed >= gesture.holdTimeMs) {

						gestureStartTime.erase(gesture.name);//reset po detekcii
						return gesture.name;
					}
				}
			}
			else {
				gestureStartTime.erase(gesture.name);//ak gesto nepasuje, reset casu
			}

		}
		return std::nullopt;

	}

};



#endif // !GESTURERECOGNIZER_HPP
