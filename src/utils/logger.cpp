// logger.cpp - Logger package
//
// Author:  Tim Stark
// Date:    May 19, 2022

#include "main/core.h"
#include "utils/logger.h"

Logger *Logger::logger = nullptr;

Logger *Logger::create(levelType logType, logStream &log, logStream &err)
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
    auto &out = (level <= logWarning || level == logDebug) ? outError : outLog;   
    fmt::vprint(out, format, args);
}