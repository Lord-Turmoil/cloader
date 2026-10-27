#pragma once

#include <stdexcept>
#include <string>

namespace cloader
{

class CloaderException : public std::exception
{
public:
    CloaderException();
    CloaderException(const char* message);
    CloaderException(std::string message);

    ~CloaderException() override = default;

    const char* what() const noexcept override;

    const std::string& GetMessage() const noexcept;

private:
    std::string _message;
};

class InvalidArgumentException : public CloaderException
{
public:
    InvalidArgumentException(const char* message);
    InvalidArgumentException(std::string message);

    ~InvalidArgumentException() override = default;
};

class FileNotFoundException : public CloaderException
{
public:
    FileNotFoundException(const char* filepath);

    ~FileNotFoundException() override = default;
};

class ClassNotFoundException : public CloaderException
{
public:
    ClassNotFoundException(const char* className, const char* symbolName);

    ~ClassNotFoundException() override = default;
};

class ClassCastException : public CloaderException
{
public:
    ClassCastException(const std::type_info& from, const std::type_info& to);

    ~ClassCastException() override = default;
};

class InstantiationException : public CloaderException
{
public:
    InstantiationException(const char* message);
    InstantiationException(const std::type_info& type, const char* cause);
    InstantiationException(const std::type_info& type, const CloaderException& cause);
    InstantiationException(const std::type_info& type, const std::exception& cause);
    InstantiationException(const std::type_info& type, const std::exception_ptr& cause);

    ~InstantiationException() override = default;

    std::exception_ptr Cause() const noexcept
    {
        return _cause;
    }

private:
    std::exception_ptr _cause;
};

} // namespace cloader
