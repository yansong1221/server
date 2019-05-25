#pragma once

#include <boost/serialization/singleton.hpp>
#include <string>

class CLogService : public boost::serialization::singleton<CLogService>
{
public:
	CLogService();
	~CLogService();

	enum ELogLevel
	{
		eLogLevelDebug,
		eLogLevelInfo,
		eLogLevelWarning,
		eLogLevelError,
		eLogLevellogFatal
	};

public:
	void logInfo(const std::string& szString);
	void logDebug(const std::string& szString);
	void logWarning(const std::string& szString);
	void logError(const std::string& szString);
	void logFatal(const std::string& szString);

private:
	void printLog(ELogLevel eLogLevel, const std::string& szString);
};
