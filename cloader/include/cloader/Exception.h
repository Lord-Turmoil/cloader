/**
 * @file Exception.h
 * @brief Exception classes for the Cloader library.
 */

#pragma once

#include <stdexcept>
#include <string>

namespace cloader
{

/**
 * @brief Base class for all exceptions thrown by the Cloader library.
 */
class CloaderException : public std::exception
{
public:
    CloaderException();
    CloaderException(const char* message);
    CloaderException(std::string message);

    ~CloaderException() override = default;

    /**
     * @brief Get the exception message compatible with std::exception.
     * @details This returns the same message as \ref GetMessage(), but in a C-style string format.
     * @return The exception message as a C-style string.
     */
    const char* what() const noexcept override;

    /**
     * @brief Get the exception message.
     * @return The exception message as a std::string.
     */
    const std::string& GetMessage() const noexcept;

private:
    std::string _message;
};

/**
 * @brief Exception thrown when an invalid argument is provided.
 */
class InvalidArgumentException : public CloaderException
{
public:
    InvalidArgumentException(const char* message);
    InvalidArgumentException(std::string message);

    ~InvalidArgumentException() override = default;
};

/**
 * @brief Exception thrown when the specified dynamic library file is not found.
 * @details By default Cloader looks for the library file in all system library paths. If the library file is not found,
 * perhaps you forget to add certain paths to the system library search path.
 */
class FileNotFoundException : public CloaderException
{
public:
    FileNotFoundException(const char* filepath);

    ~FileNotFoundException() override = default;
};

/**
 * @brief Exception thrown when a symbol is not found in the dynamic library.
 */
class SymbolNotFoundException : public CloaderException
{
public:
    SymbolNotFoundException(const char* className, const char* symbolName);

    ~SymbolNotFoundException() override = default;
};

/**
 * @brief Exception thrown when a class cast fails.
 * @details This exception is thrown when the class loaded does not match the class you are trying to load.
 */
class ClassCastException : public CloaderException
{
public:
    ClassCastException(const std::type_info& from, const std::type_info& to);

    ~ClassCastException() override = default;
};

/**
 * @brief Exception thrown when an instance of a class cannot be created.
 * @details This exception is thrown for various reasons. Usually it is caused by inner exceptions thrown during the
 * instantiation of the class. The cause of the exception can be retrieved using the \ref Cause() method.
 */
class InstantiationException : public CloaderException
{
public:
    InstantiationException(const char* message);
    InstantiationException(const std::type_info& type, const char* cause);
    InstantiationException(const std::type_info& type, const CloaderException& cause);
    InstantiationException(const std::type_info& type, const std::exception& cause);
    InstantiationException(const std::type_info& type, const std::exception_ptr& cause);

    ~InstantiationException() override = default;

    /**
     * @brief Get the cause of the exception.
     * @details This returns the exception pointer to the cause of the exception, if any.
     * @return The exception pointer to the cause of the exception, or nullptr if there is no cause.
     * @note The cause may be another CloaderException or any other std::exception. You can use std::rethrow_exception
     * to rethrow the cause and catch it as the appropriate exception type.
     * @see std::rethrow_exception
     * @see std::exception_ptr
     */
    std::exception_ptr Cause() const noexcept
    {
        return _cause;
    }

private:
    std::exception_ptr _cause;
};

} // namespace cloader
