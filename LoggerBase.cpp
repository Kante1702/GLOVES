#include "LoggerBase.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <string>
#include <sstream>



#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYWIN__)
#define WINDOWS
#endif

#if defined(WINDOWS)
#include <Windows.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif // _MSC_VER


using namespace std;

enum class Color
{
	Red, Orange, Green
};

string timeInfo() {

	auto now = chrono::system_clock::now();
	time_t now_time = chrono::system_clock::to_time_t(now);

	//thread safe konverzia na lokalny cas
	std::tm loctime;

#if defined (_MSC_VER)
	localtime_s(&loctime, &now_time);
#else
	localtime_r(&now_time, &loctime);
#endif

	//formatovanie do tvaru HH::MM:SS pomocou sstreamov
	//setw vytvara dve 'pozicie' ktore spravne doplnuje setfill
	std::ostringstream osStr;
	osStr << std::setw(2) << std::setfill('0') << loctime.tm_hour << ":"
		<< std::setw(2) << std::setfill('0') << loctime.tm_min << ":"
		<< std::setw(2) << std::setfill('0') << loctime.tm_sec;
	
	return osStr.str();
}




void print(const std::string& message, const Color& color) {

#if defined(WINDOWS)
	WORD mask = 0;	//16-bitove cislo pouziva sa pri nastavovani farby textu v konzole
	switch (color)
	{
	case Color::Green:
		mask = FOREGROUND_GREEN;
		break;
	case Color::Orange:
		mask = FOREGROUND_RED | FOREGROUND_INTENSITY;
		break;
	case Color::Red:
		mask = FOREGROUND_RED;
		break;
	default:
		break;
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), mask );

#else 
	unused(color);
#endif
	std::cout << timeInfo() << ":" << message << std::endl; //vypisane textu po zmene farby konzoly
#if defined(WINDOWS)
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY); //vratime povodny stav
#endif
}



LoggerBase::LoggerBase(const std::string& name) :m_name(name) {
	//////////////// nic
}

void LoggerBase::setName(const std::string& name)  {
	m_name = name;
}

std::string LoggerBase::name() const{
	return m_name;	
}


void LoggerBase::printInfo(const std::string &message) const {
	print(name() + ": " + message, Color::Green);
}

void LoggerBase::printWarning(const std::string& message) const {
	print(name() + ": " + message, Color::Orange);
}

void LoggerBase::printError(const std::string& message) const {
	print(name() + ": " + message, Color::Red);
}

void LoggerBase::waitForInput() const {
	auto message = name() + ": " + "PRESS ANY KEY TO CONTINUE";
	print(message, Color::Green);
	std::cin.get();
}


#ifdef _MSC_VER
#pragma warning(enable:4996)
#endif // _MSC_VER