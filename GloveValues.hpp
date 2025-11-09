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
	
	GestureRecognizer m_rightGestureRecognizer;
	GestureRecognizer m_leftGestureRecognizer;

public: 
	GloveValues();
	~GloveValues();
	void setOnlyBendingLog(bool value) { m_logOnlyBending = value; };
protected:

	std::map<std::string, std::shared_ptr<GSdk::Board::BoardPeripheral>> m_peripherals;
	void onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) override;
	void onPeripheralDisconnected() override;



};


#endif // !GLOVEVALUES_HPP


