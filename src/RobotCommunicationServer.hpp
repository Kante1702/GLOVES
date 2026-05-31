// RobotCommunicationServer.hpp
// TCP server pre komunikaciu s FANUC robotom.
// PC funguje ako server, robot (KAREL program captogloveslistengesture) sa pripaja ako klient.
// Prikazy su odosielane ako 3-znakove HMG retazce (napr. "110") bez znaku noveho riadku
// pretoze KAREL cita presne 3 znaky pomocou READ file_var(str::3).
//
// m_serverSocket zostava aktivny aj po odpojeni robota, takze opetovne pripojenie
// si vyzaduje iba nove zavolanie waitForRobot() bez nutnosti restartovat server.

#pragma once
#pragma comment(lib, "ws2_32.lib")
#ifndef RobotCommunicationServer_HPP
#define	RobotCommunicationServer_HPP
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

class RobotCommunicationServer {

	int m_port;
	SOCKET m_serverSocket;
	SOCKET m_clientSocket;
	bool m_connected;

public: 
	RobotCommunicationServer(int port);
	~RobotCommunicationServer();

	bool startServer();								// bind a listen na danom porte
	bool waitForRobot();							// blokujuce cakanie na pripojenie robota (accept)
	
	
	bool sendCommand(const std::string& comand);	 // Odosle 3-znakovy HMG prikaz robotovi; pri chybe oznaci spojenie ako prerusene
	
	std::string receiveData();
	void stopServer();


};
#endif // !RobotCommunicationServer_HPP

