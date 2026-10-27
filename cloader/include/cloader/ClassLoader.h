#pragma once

#include "cloader/Loadable.h"

#include "cloader/Exception.h"
#include "cloader/details/Logging.h"

#include <exception>
#include <format>
#include <memory>
#include <string>

namespace cloader
{

class ClassLoaderBase
{
public:
    using FactoryFn = cloader::Loadable* (*)(void* args);

    ClassLoaderBase(const char* filepath, const char* className);
    ClassLoaderBase(FactoryFn factory);

    ClassLoaderBase(const ClassLoaderBase&) = delete;
    ClassLoaderBase& operator=(const ClassLoaderBase&) = delete;
    ClassLoaderBase(ClassLoaderBase&&) = delete;
    ClassLoaderBase& operator=(ClassLoaderBase&&) = delete;

    ~ClassLoaderBase();

protected:
    Loadable* _NewInstance(void* args);

private:
    void* _handle;
    FactoryFn _factory;
};

template <typename T> class ClassLoader : public ClassLoaderBase
{
public:
    ClassLoader(const char* filepath, const char* className);
    ClassLoader(FactoryFn factory);

    std::unique_ptr<T> NewInstance(void* args);
};

template <typename T>
ClassLoader<T>::ClassLoader(const char* filepath, const char* className) : ClassLoaderBase(filepath, className)
{
    details::logging::LogInfo(std::format("Loaded class '{}' in '{}'", className, filepath));
}

template <typename T> ClassLoader<T>::ClassLoader(FactoryFn factory) : ClassLoaderBase(factory)
{
    details::logging::LogInfo(std::format("Loaded internal class '{}'", typeid(T).name()));
}

template <typename T> std::unique_ptr<T> ClassLoader<T>::NewInstance(void* args)
{
    Loadable* loadable;

    try
    {
        loadable = _NewInstance(args);
    }
    catch (InstantiationException& e)
    {
        throw InstantiationException(typeid(T), e);
    }
    catch (std::exception& e)
    {
        throw InstantiationException(typeid(T), e);
    }
    catch (...)
    {
        throw InstantiationException(typeid(T), std::current_exception());
    }

    if (!loadable)
    {
        throw InstantiationException(typeid(T), "Instantiated class is null");
    }

    T* instance = dynamic_cast<T*>(loadable);
    if (!instance)
    {
        delete loadable;
        throw ClassCastException(typeid(*loadable), typeid(T));
    }

    return std::unique_ptr<T>(instance);
}

} // namespace cloader
