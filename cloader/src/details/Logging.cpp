#include "cloader/details/Logging.h"

#include "cloader/details/Config.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

#ifdef CLOADER_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

namespace
{

cloader::details::logging::LogLevel ParseLogLevel(const char* value)
{
    if (value == nullptr)
    {
        return cloader::details::logging::LogLevel::Disabled;
    }

    std::string level(value);
    std::transform(level.begin(), level.end(), level.begin(), [](char c) { return static_cast<char>(tolower(c)); });

    if (level == "debug" || level == "d" || level == "verbose" || level == "v")
    {
        return cloader::details::logging::LogLevel::Debug;
    }
    else if (level == "info" || level == "i")
    {
        return cloader::details::logging::LogLevel::Info;
    }
    else if (level == "warning" || level == "warn" || level == "w")
    {
        return cloader::details::logging::LogLevel::Warning;
    }
    else if (level == "error" || level == "e")
    {
        return cloader::details::logging::LogLevel::Error;
    }
    else
    {
        return cloader::details::logging::LogLevel::Disabled;
    }
}

const char* ToString(cloader::details::logging::LogLevel level)
{
    switch (level)
    {
    case cloader::details::logging::LogLevel::Debug:
        return "DEBUG";
    case cloader::details::logging::LogLevel::Info:
        return "INFO";
    case cloader::details::logging::LogLevel::Warning:
        return "WARNING";
    case cloader::details::logging::LogLevel::Error:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

cloader::details::logging::LogLevel sLogLevel = cloader::details::logging::LogLevel::Unknown;

} // namespace

namespace cloader::details
{

namespace logging
{

LogLevel CurrentLogLevel()
{
    if (sLogLevel == LogLevel::Unknown)
    {
#ifdef CLOADER_PLATFORM_WINDOWS
        char* env;
        if (_dupenv_s(&env, nullptr, CLOADER_LOG_LEVEL_ENV_VAR))
        {
            sLogLevel = LogLevel::Disabled;
        }
        else
        {
            sLogLevel = ParseLogLevel(env);
            free(env);
        }
#else
        const char* env = getenv(CLOADER_LOG_LEVEL_ENV_VAR);
        sLogLevel = ParseLogLevel(env);
#endif
    }
    return sLogLevel;
}

void LogDebug(const std::string& message)
{
    Log(LogLevel::Debug, message);
}

void LogInfo(const std::string& message)
{
    Log(LogLevel::Info, message);
}

void LogWarning(const std::string& message)
{
    Log(LogLevel::Warning, message);
}

void LogError(const std::string& message)
{
    Log(LogLevel::Error, message);
}

void Log(LogLevel level, const std::string& message)
{
    if (level >= CurrentLogLevel())
    {
        const char* tag = ToString(level);
        std::cerr << "(cloader) [" << tag << "] " << message << std::endl;
    }
}

} // namespace logging

} // namespace cloader::details
