// GloveValues.hpp
// Jadro systemu - spracovanie dat rukavic, kalibracia, rozpoznavanie gest a riadenie robota.
// Rozsiruje GloveConnection o spracovanie BT dat, normalizaciu, rozpoznavanie gest
// a odosielanie prikazov robotovi cez TCP.
//
// Tok spracovania pri kazdom BT frame:
//   subscribe() callback -> logRawToCSV() -> logToCSV() ->
//   normalize() -> recognize() -> handleLeft/RightGesture() -> sendCommand()

#pragma once

#ifndef GLOVEVALUES_HPP
#define	GLOVEVALUES_HPP

#include "GestureExperiment.hpp"
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

	//--- Experimentalny rezim ---
	GestureExperiment m_experiment;	
	bool m_experimentMode = false; // ak true, gesta su smerovane do experimentu namiesto robota
	

	// --- BT streamovanie a logovanie ---
    // Registruje callback pre prichadzajuce data z danej rukavice
	void subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board);

	std::map<std::string, int> m_streamIDs;		// nazov rukavice -> ID streamu (pre odpojenie callbacku)
	std::map<std::string, GSdk::BoardTools::WearingPosition> m_glovePositions;
	std::ofstream m_logFile;
	
	// Zapisuje normalizovane hodnoty do CSV a riadi kalibraciu a rozpoznavanie gest
	void logToCSV(const std::string& gloveName, const std::vector<uint8_t> values, GSdk::BoardTools::WearingPosition position);
	
	// Extrahuje hodnoty 5 prstov zo surovych BT dat v HW poradí senzorov
	std::array<float, 5> getNormalizedFingerValues(const std::vector<uint8_t>& values, GSdk::BoardTools::ExternalSensorAssembly assembly);
	
	// --- Spracovanie gest ---
	void handleLeftGesture(const std::string& gesture);
	void handleRightGesture(const std::string& gesture);
	
	// Skontroluje ci je dané gesto pravej ruky povolene v aktualnom rezime
	bool isRightGestureAllowed(const std::string& gesture) const;
	bool m_collecting = false;

	std::map<std::string, std::ofstream> m_logFiles;	// CSV subory pre normalizovane hodnoty
	std::map<std::string, bool> m_headerWritten;

	
	std::map<std::string, std::ofstream> m_rawLogFiles;  // CSV subory pre surove hodnoty senzorov
	std::map<std::string, bool> m_rawHeaderWritten;
	
	// Zapisuje surove senzorove hodnoty do CSV pre naslednu analyzu
	void logRawToCSV(const std::string& gloveName, const std::vector<uint8_t>& values, GSdk::BoardTools::WearingPosition position);
	
	// --- Stav systemu ---
	bool m_logOnlyBending = false;
	bool leftUnlocked = false;
	bool m_leftHandEnabled = true;		// false = lava ruka zamknuta gestom Lock_Unlock

	ControlMode m_currentMode = ControlMode::None;
	CalibrationState m_calibrationState = CalibrationState::Idle;
	GestureRecognizer m_rightGestureRecognizer{ HandType::Right };
	GestureRecognizer m_leftGestureRecognizer{ HandType::Left };
	std::string m_leftGloveName;
	std::string m_rightGloveName;
	std::array<float, 5> m_lastNormalizedLeft{};
	std::array<float, 5> m_lastNormalizedRight{};



	// --- TCP komunikacia s robotom ---
	std::unique_ptr<RobotCommunicationServer> m_robotServer;

	// Prevedie nazov gesta na 3-znakovy HMG kod; vrati "" ak gesto nema priradeny kod
	std::string gesturesToCommand(const std::string& gesture);

	int m_robotPort;
	std::string m_leftGloveIdStr;
	std::string m_rightGloveIdStr;

	// Nacita konfiguraciu (port, ID rukavic) zo suboru config.txt
	void loadConfig(const std::string& filename);



	// Bezpecnostne gesta - aktivne vzdy bez ohladu na aktualny rezim riadenia
	const std::unordered_set<std::string> m_globalSafetyGestures = {
		"Left_Stop_Resume",
		"Right_Stop_Resume"
	};


	// Povolene gesta pravej ruky pre kazdy rezim riadenia
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

	// Prepne logovanie iba na ohybove senzory - vynecháva IMU data
	void setOnlyBendingLog(bool value) { m_logOnlyBending = value; };

	void startCalibrating();	// spusti interaktivny 4-krokovy kalibracny proces
	void startExperiment();		// spusti experimentalnu verifikaciu (vyzaduje dokoncenu kalibraciu)
	void stopExperiment();		// predcasne zastavi experiment a ulozi vysledky
	
	// Potvrdi aktualny krok kalibracie a spusti zber vzoriek
	void confirmCalibrationStep(const std::array<float, 5>& rawleft, const std::array<float, 5>& rawright);
	
	// Odosle manualny 3-znakovy HMG prikaz robotovi (klavesa 'm' v main)
	bool sendManualCommand(const std::string& rawCommand);

	std::array<float, 5> m_lastRawLeft{};	// posledne surove hodnoty lavej ruky (pre kalibraciu)
	std::array<float, 5> m_lastRawRight{};	// posledne surove hodnoty pravej ruky (pre kalibraciu)
	CalibrationManager m_calibrationManager;
	virtual void disconnect() override;

protected:

	std::map<std::string, std::shared_ptr<GSdk::Board::BoardPeripheral>> m_peripherals;
	void onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) override;
	void onPeripheralDisconnected() override;


};


#endif // !GLOVEVALUES_HPP
