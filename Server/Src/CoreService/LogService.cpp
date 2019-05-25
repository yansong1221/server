#include "LogService.h"
#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>

CLogService::CLogService()
{
}


CLogService::~CLogService()
{
}

void CLogService::logInfo(const std::string& szString)
{
	printLog(eLogLevelInfo, szString);
}

void CLogService::logDebug(const std::string& szString)
{
	printLog(eLogLevelDebug, szString);
}

void CLogService::logWarning(const std::string& szString)
{
	printLog(eLogLevelWarning, szString);
}

void CLogService::logError(const std::string& szString)
{
	printLog(eLogLevelError, szString);
}

void CLogService::logFatal(const std::string& szString)
{
	printLog(eLogLevellogFatal, szString);
}

void CLogService::printLog(ELogLevel eLogLevel, const std::string& szString)
{
	auto nowTime = std::chrono::system_clock::now();
	std::stringstream timeFormat;
	auto t = std::chrono::system_clock::to_time_t(nowTime);
	timeFormat << std::put_time(std::localtime(&t), "[%F %T]:");

	std::string outString = timeFormat.str();
	outString += szString;

	std::cout << outString << std::endl;
}
