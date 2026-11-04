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
 * @details The logging level is read from the CLOADER_LOG environment variable on first use.
 */
namespace logging
{

/**
 * @brief Logging levels for the Cloader library.
 * @details The level is read from the CLOADER_LOG environment variable (see \ref CLOADER_LOG_LEVEL_ENV_VAR). Each
 * enumerator below lists the values that select it. The values are case-insensitive; any other value, or an unset
 * variable, disables logging (selects \ref LogLevel::Disabled).
 */
enum class LogLevel
{
    Unknown,  ///< Placeholder for the unparsed state; never returned by CurrentLogLevel().
    Debug,    ///< Selected by "debug", "d", "verbose" or "v".
    Info,     ///< Selected by "info" or "i".
    Warning,  ///< Selected by "warning", "warn" or "w".
    Error,    ///< Selected by "error" or "e".
    Disabled, ///< Logging is turned off (the default).
};

/**
 * @brief Get the current logging level.
 * @details The logging level is read from the CLOADER_LOG environment variable (see \ref CLOADER_LOG_LEVEL_ENV_VAR)
 * on the first call to this function, and cached afterwards. See \ref LogLevel for the accepted values.
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
