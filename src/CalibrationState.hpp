// CalibrationState.hpp
// Enumeracia stavov kalibracneho procesu.
// Kalibracia prebieha sekvencne v styroch krokoch - pre kazdu ruku sa zbieraju
// vzorky otvorenej aj zatvorenej polohy (past). Stav je riadeny v GloveValues.
//
// Poradie prechodov:
//   Idle -> CalibratingLeftOpen -> CalibratingLeftFist
//        -> CalibratingRightOpen -> CalibratingRightFist -> Idle


#pragma once
#ifndef CALIBRATIONSTATE_HPP
#define CALIBRATIONSTATE_HPP


enum class CalibrationState
{

	Idle,						// kalibracia neprebieha
	CalibratingLeftOpen,		// zber vzoriek lavej ruky - otvorena poloha
	CalibratingLeftFist,		// zber vzoriek lavej ruky - zatvorena poloha (past)
	CalibratingRightOpen,		// zber vzoriek pravej ruky - otvorena poloha
	CalibratingRightFist,		// zber vzoriek pravej ruky - zatvorena poloha (past)
	Done						// rezervovany stav po dokonceni

};

#endif // !CALIBRATIONSTATE_HPP

