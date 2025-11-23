#pragma once

#ifndef GLOVEVALUES_HPP
#define	GLOVEVALUES_HPP

#include "GloveConnection.hpp"
#include <map>
#include <fstream>
#include <array>
#include "BoardTools/ExternalSensorAssembly.h"
#include "BoardTools/ExternalSensor.h"
#include "BoardTools/WearingPosition.h"
#include "GestureRecognizer.hpp"
#include "CalibrationManager.hpp"
#include "CalibrationState.hpp"


class GloveValues : public GloveConnection{

	void subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board);
	void unsubscribe();
	
	std::map<std::string, int> m_streamIDs; // name -> streamID
	std::ofstream m_logFile;
	void logToCSV(const std::string& gloveName, const std::vector<uint8_t> values, GSdk::BoardTools::WearingPosition position);
	static GSdk::BoardTools::WearingPosition detectPosition(std::shared_ptr<GSdk::Board::BoardPeripheral> board);
	std::array<float,5> getNormalizedFingerValues(const std::vector<uint8_t>& values, GSdk::BoardTools::ExternalSensorAssembly assembly );
	

	std::map<std::string, std::ofstream> m_logFiles;
	std::map<std::string, bool> m_headerWritten;
	
	bool m_logOnlyBending = false;
	
	CalibrationState m_calibrationState = CalibrationState::Idle;
	GestureRecognizer m_rightGestureRecognizer;
	GestureRecognizer m_leftGestureRecognizer;
	std::string m_leftGloveName;
	std::string m_rightGloveName;
	const char* EXPECTED_LEFT_GLOVE = "CaptoGlove4305";
	const char* EXPECTED_RIGHT_GLOVE = "CaptoGlove4272";

public: 
	GloveValues();
	~GloveValues();
	void setOnlyBendingLog(bool value) { m_logOnlyBending = value; };
	void startCalibrating();
	void confirmCalibrationStep(const std::array<float, 5>& rawleft, const std::array<float, 5>& rawright);
	std::array<float, 5> m_lastRawLeft{};
	std::array<float, 5> m_lastRawRight{};
	CalibrationManager m_calibrationManager;
protected:

	std::map<std::string, std::shared_ptr<GSdk::Board::BoardPeripheral>> m_peripherals;
	void onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) override;
	void onPeripheralDisconnected() override;
	

};


#endif // !GLOVEVALUES_HPP


