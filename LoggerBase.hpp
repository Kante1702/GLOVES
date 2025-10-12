#pragma once
#ifndef LOGGERBASE_HPP
#define LOGGERBASE_HPP

#include "GSdkCore.h"
#include <string>

class LoggerBase {

private:
	std::string m_name;

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



