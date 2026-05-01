// ControlMode.hpp
// Enumeracia rezimov riadenia robota.
// Aktivny rezim je nastavovany gestami lavej ruky a urcuje, ktore gesta
// pravej ruky su v danom momente povolene (pozri m_allowedRightGesture v GloveValues).
//


#pragma once
#ifndef CONTROLMODE_HPP
#define CONTROLMODE_HPP

enum class ControlMode {

	None,			// ziadny rezim - gesta pravej ruky su blokovane
	XYZ ,			// translacny pohyb robota po osiach X, Y, Z
	Rotation ,		// rotacny pohyb: Yaw, Pitch, Roll
	Custom ,		// uzivatelsky rezim - specialne prikazy
	Service			// servisny rezim - rezervovany pre buduci rozvoj

};


#endif // !CONTROLMODE_HPP
