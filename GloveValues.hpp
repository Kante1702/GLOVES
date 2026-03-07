
#pragma once

#ifndef GLOVEVALUES_HPP
#define	GLOVEVALUES_HPP

#include "GloveConnection.hpp"
#include "HandType.hpp"
#include "ControlMode.hpp"
#include "RobotCommunicationServer.hpp"
#include <map>
#include <unordered_set>
#include <fstream>
#include <string>
#include <array>
#include "BoardTools/ExternalSensorAssembly.h"
#include "BoardTools/ExternalSensor.h"
#include "BoardTools/WearingPosition.h"
#include "GestureRecognizer.hpp"
#include "CalibrationManager.hpp"
#include "CalibrationState.hpp"


class GloveValues : public GloveConnection {

	void subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board);


	std::map<std::string, int> m_streamIDs; // name -> streamID
	bool m_streamConnected = false;
	int m_globalStreamID = -1;
	std::map<std::string, GSdk::BoardTools::WearingPosition> m_glovePositions;
	std::ofstream m_logFile;
	void logToCSV(const std::string& gloveName, const std::vector<uint8_t> values, GSdk::BoardTools::WearingPosition position);
	std::array<float, 5> getNormalizedFingerValues(const std::vector<uint8_t>& values, GSdk::BoardTools::ExternalSensorAssembly assembly);
	void handleLeftGesture(const std::string& gesture);
	void handleRightGesture(const std::string& gesture);
	bool isRightGestureAllowed(const std::string& gesture) const;
	bool m_collecting = false;

	std::map<std::string, std::ofstream> m_logFiles;
	std::map<std::string, bool> m_headerWritten;

	///////////////raw data
	std::map<std::string, std::ofstream> m_rawLogFiles;
	std::map<std::string, bool> m_rawHeaderWritten;
	void logRawToCSV(const std::string& gloveName, const std::vector<uint8_t>& values, GSdk::BoardTools::WearingPosition position);
	//////

	bool m_logOnlyBending = false;
	bool leftUnlocked = false;
	bool m_leftHandEnabled = true;

	ControlMode m_currentMode = ControlMode::None;
	CalibrationState m_calibrationState = CalibrationState::Idle;
	GestureRecognizer m_rightGestureRecognizer{ HandType::Right };
	GestureRecognizer m_leftGestureRecognizer{ HandType::Left };
	std::string m_leftGloveName;
	std::string m_rightGloveName;
	std::array<float, 5> m_lastNormalizedLeft{};
	std::array<float, 5> m_lastNormalizedRight{};



	//pripojenie rukavice s robotom (PC = server , Robot = Client)
	std::unique_ptr<RobotCommunicationServer> m_robotServer;
	std::string gesturesToCommand(const std::string& gesture);

	int m_robotPort;
	std::string m_leftGloveIdStr;
	std::string m_rightGloveIdStr;

	void loadConfig(const std::string& filename);



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// GLOBAL (SAFETY) GESTURE
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	const std::unordered_set<std::string> m_globalSafetyGestures = {
		"Left_Stop_Resume",
		"Right_Stop_Resume"
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// ALLOWED GESTURE
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	std::unordered_map < ControlMode, std::unordered_set<std::string>> m_allowedRightGesture = {

		{
			ControlMode::XYZ,{
				"Right_X", "Right_Y" ,"Right_Z"
			}
		},
		{
			ControlMode::Rotation,{
				"Right_Yaw" , "Right_Pitch" , "Right_Roll"
			}
		},
		{
			ControlMode::Custom,{
				"Right_Custom_Mode" , "Right_RobotDisconnect"
			}
		},
	};

public:
	GloveValues();
	~GloveValues();
	void setOnlyBendingLog(bool value) { m_logOnlyBending = value; };
	void startCalibrating();
	void confirmCalibrationStep(const std::array<float, 5>& rawleft, const std::array<float, 5>& rawright);
	std::array<float, 5> m_lastRawLeft{};
	std::array<float, 5> m_lastRawRight{};
	CalibrationManager m_calibrationManager;
	virtual void disconnect() override;
protected:

	std::map<std::string, std::shared_ptr<GSdk::Board::BoardPeripheral>> m_peripherals;
	void onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) override;
	void onPeripheralDisconnected() override;


};


#endif // !GLOVEVALUES_HPP
