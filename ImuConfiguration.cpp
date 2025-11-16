#include "ImuConfiguration.hpp"
#include "GloveValues.hpp"

ImuConfiguration::ImuConfiguration() {
	
	LoggerBase::setName("ImuConfiguration");
	this->printInfo("IMU configuration starting");


}

ImuConfiguration::~ImuConfiguration() {

	this->printInfo("IMU configuration closing");

}


void ImuConfiguration::readImu() {

	if (m_peripherals.empty()) {
		this->printError("Failed to read imu descriptor");
		return;
	}

	for (auto& [gloveName, board] : m_peripherals) {
		if (!board) {
			this->printError("[ " + gloveName + " ]" + "Invalid peripheral reference");
			continue;
		}
		//Tým pádom len odkážeš na existujúci objekt, ktorý už SDK vytvorilo, a nebude sa pokúša ho inštanciova.
		auto& imuConfig = board->imuConfig();

		if (!imuConfig.read()) {
			this->printError("[ " + gloveName + " ]" + "Failed to reda IMU config");
		}
		else {
			this->printInfo("[ " + gloveName + " ]" + "Imu config successfully read");
		}
	}

}