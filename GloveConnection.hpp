#pragma once
#ifndef GloveConnection_HPP
#define GloveConnection_HPP

#include <iostream>
#include <memory> //unique_ptr,shared_ptr,weak_ptr

//LoggerBase.hpp obsahuje hlavnu lib => GSdkCore.h
#include "LoggerBase.hpp"

class GloveConnection: public LoggerBase {

public: 
	GloveConnection();
	~GloveConnection();


	virtual bool connect();
	virtual void disconnect();

protected:

	//hooky pre dalsie zdrojaky 
	virtual void onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) {};
	virtual void onPeripheralDisconnected() {};



	GSdk::Board::PeripheralCentralPtr m_central; //central
	std::shared_ptr<GSdk::Board::BoardPeripheral> m_peripheral; //rukavica
	

};



#endif // !GloveConnection_HPP
