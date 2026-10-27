#include "cloader/Exception.h"

namespace cloader
{

namespace
{

std::string FormatClassNotFoundMessage(const char* className, const char* symbolName)
{
    return "Class '" + std::string(className) + "' not found (requires symbol: '" + std::string(symbolName) + "')";
}

std::string FormatFileNotFoundMessage(const char* filepath)
{
    return "File '" + std::string(filepath) + "' not found";
}

std::string FormatClassCastMessage(const std::type_info& from, const std::type_info& to)
{
    return "Cannot cast class '" + std::string(from.name()) + "' to class '" + std::string(to.name()) + "'";
}

std::string FormatInstantiationMessage(const std::type_info& type)
{
    return "Failed to instantiate class '" + std::string(type.name()) + "'";
}

std::string FormatInstantiationMessageWithCause(const std::type_info& type, const char* cause)
{
    return FormatInstantiationMessage(type) + "': " + std::string(cause);
}

std::string FormatInstantiationMessageWithCause(const std::type_info& type, const CloaderException& cause)
{
    return FormatInstantiationMessage(type) + "': " + cause.GetMessage();
}

std::string FormatInstantiationMessageWithCause(const std::type_info& type, const std::exception& cause)
{
    return FormatInstantiationMessage(type) + "': " + std::string(cause.what());
}

} // namespace

CloaderException::CloaderException(const char* message) : _message(message ? message : "")
{
}

CloaderException::CloaderException(std::string message) : _message(std::move(message))
{
}

const char* CloaderException::what() const noexcept
{
    return _message.c_str();
}

const std::string& CloaderException::GetMessage() const noexcept
{
    return _message;
}

InvalidArgumentException::InvalidArgumentException(const char* message) : CloaderException(message)
{
}

InvalidArgumentException::InvalidArgumentException(std::string message) : CloaderException(std::move(message))
{
}

FileNotFoundException::FileNotFoundException(const char* filepath)
    : CloaderException(FormatFileNotFoundMessage(filepath))
{
}

ClassNotFoundException::ClassNotFoundException(const char* className, const char* symbolName)
    : CloaderException(FormatClassNotFoundMessage(className, symbolName))
{
}

ClassCastException::ClassCastException(const std::type_info& from, const std::type_info& to)
    : CloaderException(FormatClassCastMessage(from, to))
{
}

InstantiationException::InstantiationException(const char* message) : CloaderException(message)
{
}

InstantiationException::InstantiationException(const std::type_info& type, const char* cause)
    : CloaderException(FormatInstantiationMessageWithCause(type, cause))
{
}

InstantiationException::InstantiationException(const std::type_info& type, const CloaderException& cause)
    : CloaderException(FormatInstantiationMessageWithCause(type, cause)), _cause(std::make_exception_ptr(cause))
{
}

InstantiationException::InstantiationException(const std::type_info& type, const std::exception& cause)
    : CloaderException(FormatInstantiationMessageWithCause(type, cause)), _cause(std::make_exception_ptr(cause))
{
}

InstantiationException::InstantiationException(const std::type_info& type, const std::exception_ptr& cause)
    : CloaderException(FormatInstantiationMessage(type)), _cause(cause)
{
}

} // namespace cloader
