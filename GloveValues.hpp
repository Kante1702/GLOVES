#pragma once

#ifndef GLOVEVALUES_HPP
#define	GLOVEVALUES_HPP

#include "GloveConnection.hpp"
#include "HandType.hpp"
#include <map>
#include <fstream>
#include <array>
#include "BoardTools/ExternalSensorAssembly.h"
#include "BoardTools/ExternalSensor.h"
#include "BoardTools/WearingPosition.h"
#include "GestureRecognizer.hpp"
#include "CalibrationManager.hpp"
#include "CalibrationState.hpp"


class GloveValues : public GloveConnection {

	void subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board);
	void unsubscribe();

	std::map<std::string, int> m_streamIDs; // name -> streamID
	std::ofstream m_logFile;
	void logToCSV(const std::string& gloveName, const std::vector<uint8_t> values, GSdk::BoardTools::WearingPosition position);
	std::array<float, 5> getNormalizedFingerValues(const std::vector<uint8_t>& values, GSdk::BoardTools::ExternalSensorAssembly assembly);
	void updateGestures() {};
	void handleLeftGesture(const std::string& gestureName, const std::array<float, 5>& values) {}; //konkretne roboticke aplikacie pre lavu ruku 
	void handleRightControl(const std::string& gestureName, const std::array<float, 5>& values) {};//konkretne roboticke aplikacie pre pravu ruku 


	std::map<std::string, std::ofstream> m_logFiles;
	std::map<std::string, bool> m_headerWritten;

	bool m_logOnlyBending = false;
	bool leftUnlocked = false;
	bool m_leftHandEnabled = true;

	CalibrationState m_calibrationState = CalibrationState::Idle;
	GestureRecognizer m_rightGestureRecognizer{ HandType::Right };
	GestureRecognizer m_leftGestureRecognizer{HandType::Left};
	std::string m_leftGloveName;
	std::string m_rightGloveName;
	std::array<float, 5> m_lastNormalizedLeft{};
	std::array<float, 5> m_lastNormalizedRight{};


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


