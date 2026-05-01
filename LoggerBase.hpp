// LoggerBase.hpp
// Abstraktna zakladna trieda pre vsetky komponenty systemu.
// Poskytuje farebny konzolovy vystup s casovou znackou a definuje rozhranie
// pre pripojenie a odpojenie zariadeni (connect/disconnect).
// Farby: zelena = info, oranzova = varovanie, cervena = chyba.
//
// Dedicska hierarchia systemu:
//   LoggerBase -> GloveConnection -> GloveValues -> ImuConfiguration

#pragma once
#ifndef LOGGERBASE_HPP
#define LOGGERBASE_HPP

#include "GSdkCore.h"
#include <string>

class LoggerBase {

private:
	std::string m_name; // nazov modulu zobrazovany v logovacich spravach

public:
	virtual bool connect() = 0;
	virtual void disconnect() = 0;

	LoggerBase(const std::string& name);

	void setName(const std::string& name);
	std::string name() const;
	void printInfo(const std::string &message) const;
	void printWarning(const std::string& message) const;
	void printError(const std::string& message) const;
	void waitForInput() const;

};


#endif // !LOGGERBASE_HPP



