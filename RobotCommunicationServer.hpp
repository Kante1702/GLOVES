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

	bool startServer();
	bool waitForRobot();
	bool sendCommand(const std::string& commnad);
	std::string receiveData();
	void stopServer();


};
#endif // !RobotCommunicationServer_HPP

