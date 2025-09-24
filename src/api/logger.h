// logger.h - Logger package
//
// Author:  Tim Stark
// Date:    Sep 20, 2022

#pragma once

#include "api/ofsapi.h"

#include <iostream>
#include <fstream>
#include <fmt/format.h>
#include <fmt/ostream.h>

#pragma once

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

    Logger(levelType logType, const fs::path &logName)
    : level(logType), outLog(std::clog), outError(std::cerr)
    {
        outLogFile.open(logName);
    }

    ~Logger()
    {
        if (outLogFile.is_open())
            outLogFile.close();
    }

    inline void setLevel(levelType nLevel)      { level = nLevel; }

    // void vlog(levelType type, fmt::string_view format, fmt::format_args args) const;

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

    // void logMatrix(const glm::dmat4 &m, cstr_t &desc)
    // {
    //     debug("{} matrix:\n", desc);
    //     debug("{} {} {} {}\n", m[0][0], m[0][1], m[0][2], m[0][3]);
    //     debug("{} {} {} {}\n", m[1][0], m[1][1], m[1][2], m[1][3]);
    //     debug("{} {} {} {}\n", m[2][0], m[2][1], m[2][2], m[2][3]);
    //     debug("{} {} {} {}\n", m[3][0], m[3][1], m[3][2], m[3][3]);
    // }

protected:
    inline void vlog(levelType level, fmt::string_view format, fmt::format_args args) const
    {
        if (outLogFile.is_open())
            outLogFile << fmt::vformat(format, args) << std::flush;
        else
        {
            auto &out = (level <= logWarning || level == logDebug) ? outError : outLog;   
            fmt::vprint(out, format, args);
        }
    }

private:
    levelType level = logInfo;
    mutable logStream outLogFile;
    outStream &outLog;
    outStream &outError;
};