// logger.h - Logger package
//
// Author:  Tim Stark
// Date:    May 19, 2022

#pragma once

#include <iostream>
#include <fstream>
#include <fmt/format.h>
#include <fmt/ostream.h>

class Logger
{
public:
    using outStream = std::basic_ostream<char>;
    using logStream = std::basic_ofstream<char>;

    enum levelType
    {
        logFatal = 0,
        logError,
        logWarning,
        logInfo,
        logVerbose,
        logDebug
    };

    Logger()
    : outLog(std::clog), outError(std::cerr)
    { }

    Logger(levelType logType, outStream &log, outStream &err)
    : level(logType), outLog(log), outError(err)
    { }

    Logger(levelType logType, const fs::path &logName);
    ~Logger();

    static Logger *create(levelType type, const fs::path &logName);
    static Logger *create(levelType logType, outStream &log, outStream &err);
    static Logger *create(levelType logType);

    static void destroyLogger()     { delete Logger::logger; }
    static Logger *getLogger()      { return Logger::logger; }

    inline void setLevel(levelType nLevel)      { level = nLevel; }

    void vlog(levelType type, fmt::string_view format, fmt::format_args args) const;

    template <typename... Args>
    void log(levelType logType, cchar_t *format, const Args&... args) const
    {
        if (logType <= level)
            vlog(logType, fmt::string_view(format), fmt::make_format_args(args...));
    }

    template <typename... Args>
    inline void fatal(cchar_t *format, const Args&... args) const
    {
        log(logFatal, format, args...);
    }

    template <typename... Args>
    inline void error(cchar_t *format, const Args&... args) const
    {
        log(logError, format, args...);
    }

    template <typename... Args>
    inline void warn(cchar_t *format, const Args&... args) const
    {
        log(logWarning, format, args...);
    }

    template <typename... Args>
    inline void info(cchar_t *format, const Args&... args) const
    {
        log(logInfo, format, args...);
    }

    template <typename... Args>
    inline void verbose(cchar_t *format, const Args&... args) const
    {
        log(logVerbose, format, args...);
    }

    template <typename... Args>
    inline void debug(cchar_t *format, const Args&... args) const
    {
        log(logDebug, format, args...);
    }

    static Logger *logger;

private:
    levelType level = logInfo;
    mutable logStream outLogFile;
    outStream &outLog;
    outStream &outError;
};