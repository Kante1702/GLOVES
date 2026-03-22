#include "RobotCommunicationServer.hpp"
#include <iostream>

RobotCommunicationServer::RobotCommunicationServer(int port) : m_port(port), m_connected(false), m_serverSocket(INVALID_SOCKET), m_clientSocket(INVALID_SOCKET){

	WSAData ws;
	WSAStartup(MAKEWORD(2, 2), &ws);
	m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);

}


RobotCommunicationServer::~RobotCommunicationServer() {

	stopServer();
	WSACleanup();
}

bool RobotCommunicationServer::startServer() {

	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(m_port);
	server.sin_addr.s_addr = INADDR_ANY;

	if (bind(m_serverSocket, (sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		std::cout<<"BIND FAILED"<<std::endl;
		return false;
	}
	
	if (listen(m_serverSocket, 1) == SOCKET_ERROR) {
		std::cout << "LISTEN FAILED\n";
		return false;
	}

	std::cout << "SERVER WAITING FOR ROBOT..\n";
	return true;

}

bool RobotCommunicationServer::waitForRobot() {

	sockaddr_in client;
	int clientSize = sizeof(client);
	m_clientSocket = accept(m_serverSocket, (sockaddr*)&client, &clientSize);

	if (m_clientSocket == INVALID_SOCKET) {
		std::cout << "ACCEPT FAILED" << std::endl;
		return false;
	}
	m_connected = true;
	std::cout << "ROBOT CONNECTED" << std::endl;
	return true;

}

bool RobotCommunicationServer::sendCommand(const std::string& command) {
	if (!m_connected) {
		return false;
	}
	if (m_clientSocket == INVALID_SOCKET) {
		return false;
	}

	int result = send(m_clientSocket, command.c_str(), command.length(), 0);

	if (result == SOCKET_ERROR) {
		std::cout << "[TCP] Robot disconnected (send failed)\n";
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
		m_connected = false;
		return false;
	}
	return true;
}

std::string RobotCommunicationServer::receiveData() {

	char buffer[1024];
	int bytes = recv(m_clientSocket, buffer, sizeof(buffer), 0);

	if (bytes == 0) {
		std::cout << "[TCP] Robot disconnected\n";
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
		m_connected = false;
		return "";
	}

	if (bytes == SOCKET_ERROR) {
		std::cout << "[TCP] Robot disconnected (recv error)\n";
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
		m_connected = false;
		return "";
	}
	return std::string(buffer, bytes);
}

void RobotCommunicationServer::stopServer() {
	if (m_connected) {
		closesocket(m_clientSocket);
		m_clientSocket = INVALID_SOCKET;
		m_connected = false;
	}

	if (m_serverSocket != INVALID_SOCKET) {
		closesocket(m_serverSocket);
		m_serverSocket = INVALID_SOCKET;
	}
}