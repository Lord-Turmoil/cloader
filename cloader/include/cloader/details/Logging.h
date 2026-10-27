#pragma once

#include <string>

namespace cloader::details
{

namespace logging
{

enum class LogLevel
{
    Unknown,
    Debug,
    Info,
    Warning,
    Error,
    Disabled,
};

LogLevel CurrentLogLevel();

void Log(LogLevel level, const std::string& message);

void LogDebug(const std::string& message);
void LogInfo(const std::string& message);
void LogWarning(const std::string& message);
void LogError(const std::string& message);

} // namespace logging

} // namespace cloader::details
