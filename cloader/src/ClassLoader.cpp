#include "cloader/ClassLoader.h"

#include "cloader/Exception.h"
#include "cloader/details/Logging.h"
#include "cloader/details/NativeLoader.h"

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
    HandleGuard handle(details::LoadLibrary(filepath));
    void* symbol = details::LoadSymbolFromLibrary(handle.Get(), className);

    _handle = handle.Release();
    _factory = reinterpret_cast<cloader::Loadable* (*)(void*)>(symbol);
}

ClassLoaderBase::ClassLoaderBase(FactoryFn factory) : _handle(nullptr), _factory(factory)
{
}

ClassLoaderBase::~ClassLoaderBase()
{
    if (_handle)
    {
        details::UnloadLibrary(_handle);
        _handle = nullptr;
        _factory = nullptr;
    }
}

Loadable* ClassLoaderBase::_NewInstance(void* args)
{
    if (!_factory)
    {
        throw InstantiationException("Library not loaded");
    }
    return _factory(args);
}

} // namespace cloader
