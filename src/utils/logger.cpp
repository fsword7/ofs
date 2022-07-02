// logger.cpp - Logger package
//
// Author:  Tim Stark
// Date:    May 19, 2022

#include "main/core.h"
#include "utils/logger.h"

Logger *Logger::logger = nullptr;

Logger::Logger(levelType type, const fs::path &logName)
: level(type), outLog(std::clog), outError(std::cerr)
{
    outLogFile.open(logName);
}

Logger::~Logger()
{
    if (outLogFile.is_open())
        outLogFile.close();
}

Logger *Logger::create(levelType type, const fs::path &logName)
{
    if (Logger::logger == nullptr)
        Logger::logger = new Logger(type, logName);
    return Logger::logger;
}

Logger *Logger::create(levelType logType, outStream &log, outStream &err)
{
    if (Logger::logger == nullptr)
        Logger::logger = new Logger(logType, log, err);
    return Logger::logger;
}

Logger *Logger::create(levelType logType)
{
    return create(logType, std::clog, std::cerr);
}

void Logger::vlog(levelType level, fmt::string_view format, fmt::format_args args) const
{
    if (outLogFile.is_open())
        outLogFile << fmt::vformat(format, args) << std::flush;
    else
    {
        auto &out = (level <= logWarning || level == logDebug) ? outError : outLog;   
        fmt::vprint(out, format, args);
    }
}