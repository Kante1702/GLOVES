#pragma once
#ifndef GESTURERECOGNIZER_HPP
#define GESTURERECOGNIZER_HPP
#include "GestureLibrary.hpp"
#include <optional>
#include <string>
#include <array>


class GestureRecognizer {

	GestureLibrary library;

public:
	GestureRecognizer() :library() {}

	std::optional<std::string> recognize(const std::array<float, 5>& fingerValues , GSdk::BoardTools::WearingPosition position) {
		
		std::array<float, 5> ordereValues = fingerValues;

		if (position == GSdk::BoardTools::WearingPosition::GSdkWearingPositionLeftGlove) {
			std::reverse(ordereValues.begin(), ordereValues.end());
		}

		for (const auto& gesture : library.getGesture()) {

			bool matching = true;

			for (size_t i = 0; i < 5; ++i) {
				if (ordereValues[i] < gesture.lowerTresholds[i] || ordereValues[i] > gesture.upperTresholds[i]) {
					matching = false;
					break;
				}
			}

			if (matching) {
				return gesture.name;
			}

		}
		return std::nullopt;

	}

};



#endif // !GESTURERECOGNIZER_HPP
