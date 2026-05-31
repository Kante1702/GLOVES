// ImuConfiguration.hpp
// Vrcholova trieda dedicskej hierarchie - priamo instanciovana v main().
// Po pripojeni rukavice (hook onPeripheralConnected v GloveValues) automaticky
// nacita IMU konfiguraciu zo SDK objektu rukavice pomocou dynamic_cast.
// Pouzíva referenciu na existujuci SDK objekt - nevytvára novu instanciu.
//
// Dedicska hierarchia:
//   LoggerBase -> GloveConnection -> GloveValues -> ImuConfiguration

#pragma once
#ifndef IMUCONFIGURATION_HPP
#define IMUCONFIGURATION_HPP

#include "GloveValues.hpp"

class ImuConfiguration : public GloveValues {

	public:
		ImuConfiguration();
		~ImuConfiguration();
		
		// Nacita IMU konfiguraciu pre vsetky pripojene rukavice zo SDK
		void readImu();
};


#endif // !IMUCONFIGURATION_HPP
