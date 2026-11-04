/**
 * @file Logging.h
 * @brief Logging utilities for the Cloader library.
 */

#pragma once

#include <string>

namespace cloader::details
{

/**
 * @brief Logging utilities for the Cloader library.
 * @details The logging level is controlled by the environment variable.
 */
namespace logging
{

/**
 * @brief Logging levels for the Cloader library.
 */
enum class LogLevel
{
    Unknown,
    Debug,
    Info,
    Warning,
    Error,
    Disabled,
};

/**
 * @brief Get the current logging level.
 * @details The logging level is retrieved from the environment variable named \ref CLOADER_LOG_LEVEL_ENV_VAR on the
 * first call to this function. Subsequent calls will return the cached value.
 * @return The current logging level.
 */
LogLevel CurrentLogLevel();

/**
 * @brief Output a log message with the specified logging level.
 * @details The log message will be output to the standard error stream (std::cerr).
 * @param level The logging level of the message.
 * @param message The log message to output.
 */
void Log(LogLevel level, const std::string& message);

/**
 * @brief Output a debug log message.
 * @param message The debug log message to output.
 */
void LogDebug(const std::string& message);

/**
 * @brief Output an info log message.
 * @param message The info log message to output.
 */
void LogInfo(const std::string& message);

/**
 * @brief Output a warning log message.
 * @param message The warning log message to output.
 */
void LogWarning(const std::string& message);

/**
 * @brief Output an error log message.
 * @param message The error log message to output.
 */
void LogError(const std::string& message);

} // namespace logging

} // namespace cloader::details
