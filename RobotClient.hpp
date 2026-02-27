#pragma once
#ifndef ROBOTCLIENT_HPP
#define ROBOTCLIENT_HPP

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")

class RobotClient {

	std::string m_ip;
	int m_port;
	SOCKET m_socket;
	bool m_connected;

public:

	RobotClient(const std::string& ip, int port);
	~RobotClient();

	bool connectToRobot();
	bool sendCommand(const std::string& command);
	void disconnect();
	

};
#endif // !ROBOTCLIENT_HPP
