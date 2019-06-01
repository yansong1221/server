#pragma once

#include <boost/serialization/singleton.hpp>
#include <string>
#include "fmt/core.h"

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

#define CLOG_INFO(str) CLogService::get_mutable_instance().logInfo(str)
#define CLOG_DEBUG(str) CLogService::get_mutable_instance().logDebug(str)
#define CLOG_WARNING(str) CLogService::get_mutable_instance().logWarning(fmt::format("{},{},line:{},{}",__FILE__,__FUNCTION__,__LINE__,str))
#define CLOG_ERROR(str)  CLogService::get_mutable_instance().logError(fmt::format("{},{},line:{},{}",__FILE__,__FUNCTION__,__LINE__,str));
#define CLOG_FATAL(str) CLogService::get_mutable_instance().logFatal(fmt::format("{},{},line:{},{}",__FILE__,__FUNCTION__,__LINE__,str))