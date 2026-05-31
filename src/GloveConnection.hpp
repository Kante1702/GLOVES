// GloveConnection.hpp
// Sprava Bluetooth pripojenia k rukaviciam cez CaptoGlove SDK.
// Trieda skenuje BT zariadenia, pripoji rukavicu, vypne HID emulaciu
// (mys, joystick a pod.) a notifikuje odvodene triedy cez hook metody
// onPeripheralConnected a onPeripheralDisconnected.
// HID emulacia je vypnuta, pretoze system pracuje len so surovymi senzorovymi datami.

#pragma once
#ifndef GloveConnection_HPP
#define GloveConnection_HPP

#include <iostream>
#include <memory> 
#include "LoggerBase.hpp"

class GloveConnection: public LoggerBase {

	


public: 
	GloveConnection();
	~GloveConnection();


	virtual bool connect(); // spusti BT skenovanie (casovy limit 5 sekund)
	virtual void disconnect();

protected:

	// Hook volaný po uspesnom BT pripojeni rukavice - implementuje GloveValues
	virtual void onPeripheralConnected(std::shared_ptr<GSdk::Board::BoardPeripheral> board) {};
	
	// Hook volaný po odpojeni rukavice - implementuje GloveValues
	virtual void onPeripheralDisconnected() {};
	
	// Vypne vsetky HID rezime rukavice, aby nedochadzalo ku konfliktom s OS
	void disableHID(std::shared_ptr<GSdk::Board::BoardPeripheral> board);


	GSdk::Board::PeripheralCentralPtr m_central;					// BT central manager (SDK)
	std::shared_ptr<GSdk::Board::BoardPeripheral> m_peripheral;		// aktualna pripojená rukavica
	

};



#endif // !GloveConnection_HPP
