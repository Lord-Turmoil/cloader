#include "cloader/ClassLoader.h"

#include "cloader/Exception.h"
#include "cloader/details/Config.h"
#include "cloader/details/Logging.h"

#include "details/NativeLoader.h"

#include <format>
#include <iostream>
#include <stdexcept>

namespace
{

class HandleGuard
{
public:
    explicit HandleGuard(void* handle) : _handle(handle)
    {
    }

    ~HandleGuard()
    {
        if (_handle)
        {
            cloader::details::UnloadLibrary(_handle);
            _handle = nullptr;
        }
    }

    void* Get() const
    {
        return _handle;
    }

    void* Release()
    {
        void* handle = _handle;
        _handle = nullptr;
        return handle;
    }

private:
    void* _handle;
};

} // namespace

namespace cloader
{

ClassLoaderBase::ClassLoaderBase(const char* filepath, const char* className)
{
    HandleGuard handle(details::LoadLibraryFromFile(filepath));

    std::string newInstanceSymbolName = details::ResolveSymbolName(CLOADER_NEW_SYMBOL_PREFIX, className);
    std::string deleteInstanceSymbolName = details::ResolveSymbolName(CLOADER_DELETE_SYMBOL_PREFIX, className);

    void* newInstanceSymbol = details::LoadSymbolFromLibrary(handle.Get(), newInstanceSymbolName.c_str());
    if (!newInstanceSymbol)
    {
        throw SymbolNotFoundException(className, newInstanceSymbolName.c_str());
    }
    _newInstanceFn = reinterpret_cast<NewInstanceFn>(newInstanceSymbol);

    void* deleteInstanceSymbol = details::LoadSymbolFromLibrary(handle.Get(), deleteInstanceSymbolName.c_str());
    if (deleteInstanceSymbol)
    {
        _deleteInstanceFn = reinterpret_cast<DeleteInstanceFn>(deleteInstanceSymbol);
    }
    else
    {
        details::logging::LogWarning(std::format("Symbol '{}' not found for class '{}'. Using default delete function.",
                                                 deleteInstanceSymbolName, className));
        _deleteInstanceFn = nullptr;
    }

    _handle = handle.Release();
}

ClassLoaderBase::ClassLoaderBase(NewInstanceFn newInstanceFn, DeleteInstanceFn deleteInstanceFn)
    : _handle(nullptr), _newInstanceFn(newInstanceFn), _deleteInstanceFn(deleteInstanceFn)
{
    if (!_newInstanceFn)
    {
        throw InvalidArgumentException("New instance function cannot be null");
    }
}

ClassLoaderBase::ClassLoaderBase(NewInstanceFn newInstanceFn)
    : _handle(nullptr), _newInstanceFn(newInstanceFn), _deleteInstanceFn(nullptr)
{
    if (!_newInstanceFn)
    {
        throw InvalidArgumentException("New instance function cannot be null");
    }
}

ClassLoaderBase::~ClassLoaderBase()
{
    if (_handle)
    {
        details::UnloadLibrary(_handle);
        _handle = nullptr;
        _newInstanceFn = nullptr;
        _deleteInstanceFn = nullptr;
    }
}

} // namespace cloader
