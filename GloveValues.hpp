#pragma once

#ifndef GLOVEVALUES_HPP
#define	GLOVEVALUES_HPP

#include "GloveConnection.hpp"
#include <map>
#include <fstream>
#include "BoardTools/ExternalSensorAssembly.h"
#include "BoardTools/ExternalSensor.h"
#include "BoardTools/WearingPosition.h"

class GloveValues : public GloveConnection{

	void subscribe(const std::string& gloveName, std::shared_ptr<GSdk::Board::BoardPeripheral> board);
	void unsubscribe();
	
	std::map<std::string, int> m_streamIDs; // name -> streamID
	std::ofstream m_logFile;
	void logToCSV(const std::string& gloveName, const std::vector<uint8_t> values);
	static GSdk::BoardTools::WearingPosition detectPosition(std::shared_ptr<GSdk::Board::BoardPeripheral> board);

	std::map<std::string, std::ofstream> m_logFiles;
	std::map<std::string, bool> m_headerWritten;
	bool m_logOnlyBending = false;

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


