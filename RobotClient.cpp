#include "RobotClient.hpp"
#include <iostream>
#include "LoggerBase.hpp"

RobotClient::RobotClient(const std::string& ip, int port) : m_ip(ip), m_port(port), m_connected(false) {

	WSAData ws;
	WSAStartup(MAKEWORD(2, 2), &ws);
	m_socket = socket(AF_INET, SOCK_STREAM, 0);


}

RobotClient::~RobotClient() {

	disconnect();
	WSACleanup;

}

bool RobotClient::connectToRobot() {

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(m_port);
	InetPtonA(AF_INET, m_ip.c_str(), &server.sin_addr);

	if (connect(m_socket, (sockaddr*)&server, sizeof(server)) == 0) {

		m_connected = true;
		std::cout << "CONNECTED TO ROBOT" << std::endl;
		return true;

	}
	std::cout << "CONNECTION FAILED" << std::endl;
	return false;

}

bool RobotClient::sendCommand(const std::string& command) {
	if (!m_connected) {
		return false;
	}

	int result = send(m_socket, command.c_str(), command.length(), 0);
	return result != SOCKET_ERROR;
}


void RobotClient::disconnect() {

	if (m_connected) {
		closesocket(m_socket);
		m_connected = false;
	}
}
