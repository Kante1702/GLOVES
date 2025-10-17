#pragma once
#ifndef IMUCONFIGURATION_HPP
#define IMUCONFIGURATION_HPP

#include "GloveValues.hpp"

class ImuConfiguration : public GloveValues {

	public:
		ImuConfiguration();
		~ImuConfiguration();

		void readImu();
};


#endif // !IMUCONFIGURATION_HPP
